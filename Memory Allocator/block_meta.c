// SPDX-License-Identifier: BSD-3-Clause

#include "block_meta.h"
#include "osmem.h"
#include "allocator.h"

/* get total size of the memory needed to create the cell */
size_t total_cell_size(size_t size)
{
	return sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta)) +
			size + padding_bytes(size);
}

/* knowing the address of the payload, get the address of the cell */
struct block_meta *get_cell_from_payload(void *ptr)
{
	return (struct block_meta *) ((char *) ptr - padding_bytes(sizeof(struct block_meta)) -
									sizeof(struct block_meta));
}

/* knowing the address of the cell, get the address of the payload */
void *get_payload_from_cell(void *ptr)
{
	return (void *)((char *) ptr + sizeof(struct block_meta) +
							padding_bytes(sizeof(struct block_meta)));
}

/* the head of the block_meta list */
struct block_meta block_list_head;

/* determine how many bytes are needed for the padding */
int padding_bytes(size_t size)
{
	if (size % BYTE == 0)
		return 0;

	return BYTE - (size % BYTE);
}

void meta_head_init(void)
{
	block_list_head.status = -1;
	block_list_head.size = 0;
	block_list_head.next = &block_list_head;
	block_list_head.prev = &block_list_head;
}

/* execute heap preallocation */
void *block_list_prealloc(size_t size, int threshold)
{
	size_t total_size = sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta)) +
						size + padding_bytes(size);
	void *ptr = NULL;

	ptr = sbrk(threshold);
	DIE(ptr == (void *) -1, "ALLOC ERROR");

	struct block_meta *block = (struct block_meta *)ptr;

	block->next = &block_list_head;
	block->prev = block_list_head.prev;
	block_list_head.prev->next = block;
	block_list_head.prev = block;

	block->size = total_size - sizeof(struct block_meta) - padding_bytes(sizeof(struct block_meta));
	block->status = STATUS_ALLOC;

	/* split the cell if there is enough space for a free block */
	if ((threshold - total_size) >= sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta)) + 1) {
		struct block_meta *free_block = (struct block_meta *)((char *)ptr + total_size);

		free_block->next = &block_list_head;
		free_block->prev = block_list_head.prev;
		block_list_head.prev->next = free_block;
		block_list_head.prev = free_block;

		free_block->size = threshold - total_size - sizeof(struct block_meta) - padding_bytes(sizeof(struct block_meta));
		free_block->status = STATUS_FREE;
	} else {
		block->size = threshold - sizeof(struct block_meta) - padding_bytes(sizeof(struct block_meta));
	}

	return ptr;
}

/* add a cell to the block_meta list */
void *block_list_add(size_t size, size_t threshold)
{
	size_t total_size = total_cell_size(size);
	int status = 0;
	void *ptr = NULL;

	/* if the threshold is exceeded, then map the memory instead of allocating it */
	if (total_size > threshold) {
		status = STATUS_MAPPED;
		ptr = (void *)mmap(0, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		DIE(ptr == (void *) -1, "MAP ERROR");

	} else {
		status = STATUS_ALLOC;
		ptr = sbrk(total_size);
		DIE(ptr == (void *) -1, "ALLOC ERROR");
	}

	struct block_meta *block = (struct block_meta *)ptr;

	if (status == STATUS_ALLOC) {
		block->next = &block_list_head;
		block->prev = block_list_head.prev;
		block_list_head.prev->next = block;
		block_list_head.prev = block;
	} else {
		block->next = block_list_head.next;
		block->prev = &block_list_head;
		block_list_head.next->prev = block;
		block_list_head.next = block;
	}

	block->size = total_size - sizeof(struct block_meta) - padding_bytes(sizeof(struct block_meta));
	block->status = status;

	return ptr;
}

/* find the best block that fits the new size of th payload */
void *find_best_block(size_t size)
{
	struct block_meta *best_block = NULL;
	size_t best_size = 0;

	for (struct block_meta *block = block_list_head.next; block != &block_list_head; block = block->next) {
		if (block->status == STATUS_FREE && block->size >= (size + padding_bytes(size))) {
			if (best_block == NULL || best_size > block->size) {
				best_block = block;
				best_size = block->size;
			}
		}
	}
	return (void *)best_block;
}

/* reuse an already allocated block, knowing the new size of the payload */
void *block_reuse(void *ptr, size_t size)
{
	struct block_meta *block = (struct block_meta *)ptr;
	size_t former_size = block->size;

	block->status = STATUS_ALLOC;

	size_t total_size = total_cell_size(size);

	block->size = size + padding_bytes(size);

	/* split the cell if there is enough space fir a free block */
	if (former_size - block->size >= sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta)) + 1) {
		struct block_meta *new_free_block = (struct block_meta *)((char *)ptr + total_size);

		new_free_block->status = STATUS_FREE;
		new_free_block->size = former_size - block->size -
								sizeof(struct block_meta) - padding_bytes(sizeof(struct block_meta));

		new_free_block->prev = block;
		new_free_block->next = block->next;
		block->next->prev = new_free_block;
		block->next = new_free_block;
	} else {
		block->size = former_size;
	}

	return (void *)block;
}

/* combine two cells next to eachother */
void do_coalesce(void *ptr)
{
	struct block_meta *first_block = (struct block_meta *)ptr;
	struct block_meta *second_block = first_block->next;

	first_block->size += second_block->size + sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta));
	first_block->next = second_block->next;
	second_block->next->prev = first_block;
}

/* search for free blocks that can be coalesced */
void coalesce_search(void)
{
	struct block_meta *block = block_list_head.next;

	while (block != &block_list_head) {
		if (block->status == STATUS_FREE && block->next->status == STATUS_FREE)
			do_coalesce((void *)block);
		else
			block = block->next;
	}
}

/* exapnd the last block on the heap to fit a new payload size */
void expand_block(size_t size)
{
	if (size > MMAP_THRESHOLD)
		return;

	int new_size = size + padding_bytes(size);
	int former_size = block_list_head.prev->size;

	void *inter = sbrk(new_size - former_size);

	DIE(inter == (void *) -1, "ALLOC ERROR");

	block_list_head.prev->size = new_size;
	block_list_head.prev->status = STATUS_ALLOC;
}

/* check if there is any memory allocated on the heap */
int is_heap_unused(void)
{
	for (struct block_meta *block = block_list_head.next; block != &block_list_head; block = block->next) {
		if (block->status == STATUS_FREE || block->status == STATUS_ALLOC)
			return 0;
	}
	return 1;
}

/* initialise every byte of the payload with 0 */
void initialise_mem(void *ptr)
{
	struct block_meta *block = (struct block_meta *)ptr;

	for (size_t i = 0; i < block->size; i++)
		*((char *)ptr + sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta)) + i) = 0;
}

size_t min_func(size_t a, size_t b)
{
	if (a < b)
		return a;

	return b;
}
