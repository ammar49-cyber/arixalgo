#ifndef ARIX_MEMORY_H
#define ARIX_MEMORY_H

#include <stddef.h>

void* arix_malloc(size_t size, size_t alignment);
void arix_free(void* ptr, size_t size);
void* arix_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment);
void arix_secure_zero(void* ptr, size_t size);
void arix_secure_copy(void* dst, const void* src, size_t size);

#endif /* ARIX_MEMORY_H */
