#include "arix_memory.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#endif

void* arix_malloc(size_t size, size_t alignment) {
    void* ptr = NULL;
#ifdef _WIN32
    ptr = _aligned_malloc(size, alignment);
#elif defined(__linux__) || defined(__APPLE__)
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
#else
    ptr = malloc(size);
#endif
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void arix_free(void* ptr, size_t size) {
    if (!ptr) return;
    arix_secure_zero(ptr, size);
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void* arix_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    void* new_ptr = arix_malloc(new_size, alignment);
    if (!new_ptr) return NULL;
    size_t copy_size = old_size < new_size ? old_size : new_size;
    arix_secure_copy(new_ptr, ptr, copy_size);
    arix_free(ptr, old_size);
    return new_ptr;
}

void arix_secure_zero(void* ptr, size_t size) {
    if (!ptr) return;
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }
}

void arix_secure_copy(void* dst, const void* src, size_t size) {
    if (!dst || !src) return;
    memcpy(dst, src, size);
}
