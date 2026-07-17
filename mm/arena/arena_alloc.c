#include "arena_alloc.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_arena_create(size_t block_size, size_t alignment) { (void)block_size; (void)alignment; return calloc(1, 32); }
void SNEPPX_arena_destroy(void* arena) { free(arena); }
void* SNEPPX_arena_alloc(void* arena, size_t size) { (void)arena; (void)size; return NULL; }
void* SNEPPX_arena_alloc_aligned(void* arena, size_t size, size_t alignment) { (void)arena; (void)size; (void)alignment; return NULL; }
void SNEPPX_arena_reset(void* arena) { (void)arena; }
size_t SNEPPX_arena_used(void* arena) { (void)arena; return 0; }
size_t SNEPPX_arena_capacity(void* arena) { (void)arena; return 0; }
void SNEPPX_arena_stats(void* arena, size_t* total_allocated, size_t* total_used, size_t* wasted) { (void)arena; (void)total_allocated; (void)total_used; (void)wasted; }
