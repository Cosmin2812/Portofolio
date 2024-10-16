/* SPDX-License-Identifier: BSD-3-Clause */

#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define METADATA_SIZE		(sizeof(struct block_meta))
#define MOCK_PREALLOC		(128 * 1024 - METADATA_SIZE - 8)
#define MMAP_THRESHOLD		(128 * 1024)
#define MULT_KB			1024
#define BYTE 8

#define PROT_READ	0x1		/* Page can be read.  */
#define PROT_WRITE	0x2		/* Page can be written.  */
#define PROT_EXEC	0x4		/* Page can be executed.  */
#define PROT_NONE	0x0		/* Page can not be accessed.  */

/* Sharing types (must choose one and only one of these).  */
#define MAP_SHARED	0x01		/* Share changes.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */
#define MAP_ANONYMOUS	0x20		/* Don't use a file.  */
#define MAP_ANON	MAP_ANONYMOUS

#define MAP_FIXED 0x10

#define MREMAP_MAYMOVE	1

#define MAP_FAILED	((void *) -1)

extern struct block_meta block_list_head;

void meta_head_init();
void *block_list_prealloc(size_t size, int treshold);
void *block_list_add(size_t size, size_t treshold);
void *find_best_block(size_t size);
void *block_reuse(void* block, size_t size);
int padding_bytes(size_t size);
void coalesce_search();
void do_coalesce(void* ptr);
void expand_block(size_t size);
int is_heap_unused();
void initialise_mem(void *ptr);
size_t min_func(size_t a, size_t b);
size_t total_cell_size(size_t size);
struct block_meta* get_cell_from_payload(void *ptr);
void* get_payload_from_cell(void *ptr);
