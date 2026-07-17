#include "object_pool.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_objpool_create(size_t object_size, size_t capacity, int thread_safe) { (void)object_size; (void)capacity; (void)thread_safe; return calloc(1, 32); }
void SNEPPX_objpool_destroy(void* pool) { free(pool); }
void* SNEPPX_objpool_acquire(void* pool) { (void)pool; return NULL; }
int SNEPPX_objpool_release(void* pool, void* obj) { (void)pool; (void)obj; return 0; }
size_t SNEPPX_objpool_available(void* pool) { (void)pool; return 0; }
size_t SNEPPX_objpool_capacity(void* pool) { (void)pool; return 0; }
int SNEPPX_objpool_prealloc(void* pool, size_t count) { (void)pool; (void)count; return 0; }
int SNEPPX_objpool_clear(void* pool) { (void)pool; return 0; }
