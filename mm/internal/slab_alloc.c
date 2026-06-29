/*
 * Slab Allocator Implementation — SKELETON
 * VERSION: v0.5
 */

#include "slab_alloc.h"
#include <stdlib.h>
#include <string.h>

int arix_slab_cache_create(ArixSlabCache** cache, size_t block_size, size_t alignment) {
    (void)block_size; (void)alignment;
    if (!cache) return -1;
    *cache = (ArixSlabCache*)calloc(1, sizeof(ArixSlabCache));
    return *cache ? 0 : -1;
}

void arix_slab_cache_destroy(ArixSlabCache* cache) { free(cache); }

void* arix_slab_cache_alloc(ArixSlabCache* cache) { (void)cache; return NULL; }
void  arix_slab_cache_free(ArixSlabCache* cache, void* ptr) { (void)cache; (void)ptr; }
void  arix_slab_cache_gc(ArixSlabCache* cache) { (void)cache; }

void arix_slab_local_init(ArixSlabLocalCache* local, ArixSlabCache* parent) {
    if (local) { memset(local, 0, sizeof(*local)); local->parent = parent; }
}
void arix_slab_local_destroy(ArixSlabLocalCache* local) { (void)local; }
void* arix_slab_local_alloc(ArixSlabLocalCache* local) { (void)local; return NULL; }
void  arix_slab_local_free(ArixSlabLocalCache* local, void* ptr) { (void)local; (void)ptr; }
void  arix_slab_local_flush(ArixSlabLocalCache* local) { (void)local; }
