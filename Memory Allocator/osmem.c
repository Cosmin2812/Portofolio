// SPDX-License-Identifier: BSD-3-Clause

#include "osmem.h"
#include "block_meta.h"
#include "allocator.h"

void *os_malloc(size_t size)
{
	/* TODO: Implement os_malloc */

	void *ptr = NULL;

	size_t total_size = total_cell_size(size);

	if (size == 0)
		return NULL;

	/* initialise the list, preallocate memory on the heap if needed */

	if (block_list_head.next == NULL) {
		meta_head_init();
		if (total_size < MMAP_THRESHOLD)
			ptr = block_list_prealloc(size, MMAP_THRESHOLD);
		else
			ptr = block_list_add(size, MMAP_THRESHOLD);

	} else if (total_size < MMAP_THRESHOLD && is_heap_unused()) {
		ptr = block_list_prealloc(size, MMAP_THRESHOLD);
	}

	if (ptr != NULL)
		return get_payload_from_cell(ptr);

	/* coalesce any free block next to eachother */

	coalesce_search();

	/* search for a block that can fit the size of the payload */

	void *block = find_best_block(size);

	if (block == NULL) {
		/* expand the last block if it is free, or just add a new cell to the list */

		if (block_list_head.prev->status == STATUS_FREE  && total_size < MMAP_THRESHOLD) {
			expand_block(size);
			ptr = (void *) block_list_head.prev;
		} else {
			ptr = block_list_add(size, MMAP_THRESHOLD);
		}

	} else {
		/* reuse the found block */

		ptr = block_reuse(block, size);
	}

	if (ptr == NULL)
		return NULL;

	return get_payload_from_cell(ptr);
}

void os_free(void *ptr)
{
	/* TODO: Implement os_free */

	if (ptr == NULL)
		return;

	struct block_meta *block = get_cell_from_payload(ptr);

	/* unmap the whole cell or just mark the status free, if allocated */

	if (block->status == STATUS_MAPPED) {
		struct block_meta *next = block->next;
		struct block_meta *prev = block->prev;

		prev->next = next;
		next->prev = prev;

		munmap((void *) block, block->size + sizeof(struct block_meta) + padding_bytes(sizeof(struct block_meta)));
	} else {
		block->status = STATUS_FREE;
	}
}

void *os_calloc(size_t nmemb, size_t size)
{
	/* TODO: Implement os_calloc */

	size_t total_size = total_cell_size(size * nmemb);

	if (nmemb * size == 0)
		return NULL;

	void *ptr = NULL;

	/* initialise the list, preallocate memory on the heap if needed */

	if (block_list_head.next == NULL) {
		meta_head_init();

		if (total_size < (size_t) sysconf(_SC_PAGESIZE))
			ptr = block_list_prealloc(size * nmemb, MMAP_THRESHOLD);
		else
			ptr = block_list_add(size * nmemb, sysconf(_SC_PAGESIZE));

		initialise_mem(ptr);
		return get_payload_from_cell(ptr);
	}

	/*
	 * if the size exceedes the size of the page,
	 * map the memory instead of allocating it
	 */

	if (total_size < (size_t) sysconf(_SC_PAGESIZE) && is_heap_unused()) {
		ptr = block_list_prealloc(size * nmemb, MMAP_THRESHOLD);

		initialise_mem(ptr);

	} else if (total_size > (size_t) sysconf(_SC_PAGESIZE)) {
		ptr = block_list_add(size * nmemb, sysconf(_SC_PAGESIZE));

		initialise_mem(ptr);
	}

	if (ptr != NULL)
		return get_payload_from_cell(ptr);

	/* coalesce any free block next to eachother */

	coalesce_search();

	/* search for a block that can fit the size of the payload */

	void *block = find_best_block(size * nmemb);

	if (block == NULL) {
		/* expand the last block if it is free, or just add a new cell to the list */

		if (block_list_head.prev->status == STATUS_FREE) {
			expand_block(size * nmemb);
			ptr = (void *) block_list_head.prev;

		} else {
			ptr = block_list_add(size * nmemb, sysconf(_SC_PAGESIZE));
		}
		initialise_mem(ptr);

	} else {
		ptr = block_reuse(block, size * nmemb);
		initialise_mem(ptr);
	}

	if (ptr != NULL)
		return get_payload_from_cell(ptr);

	return NULL;
}

void *os_realloc(void *ptr, size_t size)
{
	/* TODO: Implement os_realloc */

	void *res = NULL;
	struct block_meta *block_res = NULL;

	if (ptr == NULL) {
		res = os_malloc(size);
		return res;
	}

	if (size == 0) {
		os_free(ptr);
		return NULL;
	}

	size_t total_size = total_cell_size(size);
	struct block_meta *block = get_cell_from_payload(ptr);
	size_t usable_size = block->size;

	if (block->status == STATUS_FREE)
		return NULL;

	/* map a new memory zone if the status is mapped, or size exceedes threshold */

	if (block->status == STATUS_MAPPED || total_size > MMAP_THRESHOLD) {
		res = os_malloc(size);

		for (size_t i = 0; i < min_func(usable_size, size); i++)
			*((char *) res + i) = *((char *) ptr + i);

		os_free(ptr);
		return res;

	} else if (block->status == STATUS_ALLOC) {
		/* reuse the same cell if possible */

		if (size + padding_bytes(size) <= block->size) {
			block_res = (struct block_meta *) block_reuse(block, size);
			res = get_payload_from_cell((void *)block_res);
			return res;
		}

		if (total_size < MMAP_THRESHOLD) {
			/* check if the block is the last one in the list */

			if (block_list_head.prev == block) {
				expand_block(size);
				res = get_payload_from_cell((void *) block);
				return res;
			}

			/* colaesce any free block next to eachother */

			coalesce_search();

			/* check if the next cell is free and has enough space to fit the payload */

			size_t merge_size = block->next->size + block->size + sizeof(struct block_meta) +
							padding_bytes(sizeof(struct block_meta));

			if (block->next->status == STATUS_FREE && merge_size >= size) {
				do_coalesce((void *) block);
				block_res = (struct block_meta *) block_reuse(block, size);
				res = get_payload_from_cell((void *) block_res);
				return res;
			}

			/* search for a block that can fit the size of the payload */

			void *block_res = find_best_block(size);

			if (block_res == NULL && block_list_head.prev->status != STATUS_FREE) {
				/* just add the cell at the end of the list */

				res = os_malloc(size);

				for (size_t i = 0; i < min_func(usable_size, size); i++)
					*((char *) res + i) = *((char *) ptr + i);

				os_free(ptr);
				return res;

			} else if (block_res != NULL) {
				/* reuse the cell that has enough space to fit size bytes */

				block_res = (struct block_meta *) block_reuse(block_res, size);
				res = get_payload_from_cell((void *) block_res);

				for (size_t i = 0; i < min_func(usable_size, size); i++)
					*((char *) res + i) = *((char *) ptr + i);

				os_free(ptr);
				return res;

			} else if (block_list_head.prev->status == STATUS_FREE) {
				/* expand the last cell to fit size bytes */

				expand_block(size);
				res = get_payload_from_cell((void *) block_list_head.prev);

				for (size_t i = 0; i < min_func(usable_size, size); i++)
					*((char *) res + i) = *((char *) ptr + i);

				os_free(ptr);
				return res;
			}
		}
	}

	return NULL;
}
