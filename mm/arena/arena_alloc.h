#ifndef SNEPPX_ARENA_ALLOC_H
#define SNEPPX_ARENA_ALLOC_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_arena_create(size_t block_size, size_t alignment);
void SNEPPX_arena_destroy(void* arena);
void* SNEPPX_arena_alloc(void* arena, size_t size);
void* SNEPPX_arena_alloc_aligned(void* arena, size_t size, size_t alignment);
void SNEPPX_arena_reset(void* arena);
size_t SNEPPX_arena_used(void* arena);
size_t SNEPPX_arena_capacity(void* arena);
void SNEPPX_arena_stats(void* arena, size_t* total_allocated, size_t* total_used, size_t* wasted);
#ifdef __cplusplus
}
#endif
#endif
