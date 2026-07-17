#ifndef SNEPPX_OBJECT_POOL_H
#define SNEPPX_OBJECT_POOL_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_objpool_create(size_t object_size, size_t capacity, int thread_safe);
void SNEPPX_objpool_destroy(void* pool);
void* SNEPPX_objpool_acquire(void* pool);
int   SNEPPX_objpool_release(void* pool, void* obj);
size_t SNEPPX_objpool_available(void* pool);
size_t SNEPPX_objpool_capacity(void* pool);
int SNEPPX_objpool_prealloc(void* pool, size_t count);
int SNEPPX_objpool_clear(void* pool);
#ifdef __cplusplus
}
#endif
#endif
