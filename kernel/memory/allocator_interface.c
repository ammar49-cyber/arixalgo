#include "polymorphic_memory_allocator.h"
#include <stdlib.h>

void* SNEPPX_malloc(size_t size, size_t alignment) {
    (void)alignment;
    return malloc(size);
}

void SNEPPX_free(void* ptr, size_t size) {
    (void)size;
    free(ptr);
}

void* SNEPPX_calloc(size_t count, size_t size, size_t alignment) {
    (void)alignment;
    return calloc(count, size);
}

void* SNEPPX_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    (void)old_size; (void)alignment;
    return realloc(ptr, new_size);
}

size_t SNEPPX_allocated_size(const void* ptr) {
    (void)ptr;
    return 0;
}

int SNEPPX_malloc_stats(size_t* total_allocated, size_t* peak_allocated, size_t* num_allocs) {
    (void)total_allocated; (void)peak_allocated; (void)num_allocs;
    return 0;
}
