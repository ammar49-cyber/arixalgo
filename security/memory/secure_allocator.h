#ifndef ARIX_SECURE_ALLOCATOR_H
#define ARIX_SECURE_ALLOCATOR_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void*    addr;
    size_t   size;
    size_t   guard_front;
    size_t   guard_back;
    uint64_t canary;
    int      is_freed;
} ArixSecureAllocRecord;

typedef struct ArixSecureAllocator {
    size_t   total_allocated;
    size_t   peak_allocated;
    size_t   num_allocations;
    int      use_guard_pages;
    int      use_canaries;
    void*    live_allocations;
    int      (*on_overflow)(const ArixSecureAllocRecord* record);
} ArixSecureAllocator;

typedef struct {
    size_t total_allocated;
    size_t peak_allocated;
    size_t num_allocations;
    size_t num_frees;
    size_t num_double_free_detected;
    size_t num_canary_violations;
} ArixSecureAllocStats;

int  arix_secure_allocator_init(ArixSecureAllocator* alloc);
void arix_secure_allocator_destroy(ArixSecureAllocator* alloc);

void* arix_secure_alloc(ArixSecureAllocator* alloc, size_t bytes, size_t alignment);
void  arix_secure_free(ArixSecureAllocator* alloc, void* ptr);
void  arix_secure_audit(ArixSecureAllocator* alloc);

uint64_t arix_secure_canary_generate(void);
int      arix_secure_canary_check(void* ptr, uint64_t canary);

int                    arix_secure_freelist_check(ArixSecureAllocator* alloc);
int                    arix_secure_free_quarantine(ArixSecureAllocator* alloc, void* ptr);
void                   arix_secure_free_flush_quarantine(ArixSecureAllocator* alloc);
ArixSecureAllocStats   arix_secure_allocator_get_stats(ArixSecureAllocator* alloc);

#ifdef __cplusplus
}
#endif
#endif
