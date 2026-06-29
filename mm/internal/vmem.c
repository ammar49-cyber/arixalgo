/*
 * Virtual Memory Implementation — SKELETON
 * VERSION: v0.5
 */

#include "vmem.h"
#include <stdlib.h>
#include <string.h>

void arix_vmem_init(ArixVMemAllocator* alloc) {
    if (alloc) memset(alloc, 0, sizeof(*alloc));
}
void arix_vmem_cleanup(ArixVMemAllocator* alloc) { (void)alloc; }
void* arix_vmem_reserve(ArixVMemAllocator* alloc, size_t bytes, size_t alignment, int flags) {
    (void)alloc; (void)bytes; (void)alignment; (void)flags; return NULL;
}
int arix_vmem_commit(ArixVMemAllocator* alloc, void* addr, size_t bytes) {
    (void)alloc; (void)addr; (void)bytes; return 0;
}
int arix_vmem_decommit(ArixVMemAllocator* alloc, void* addr, size_t bytes) {
    (void)alloc; (void)addr; (void)bytes; return 0;
}
void arix_vmem_release(ArixVMemAllocator* alloc, void* addr, size_t bytes) {
    (void)alloc; (void)addr; (void)bytes;
}
int arix_vmem_advise_hugepage(void* addr, size_t bytes) {
    (void)addr; (void)bytes; return 0;
}
int arix_vmem_advise_nohugepage(void* addr, size_t bytes) {
    (void)addr; (void)bytes; return 0;
}
int arix_vmem_register_evict_strategy(ArixVMemAllocator* alloc, ArixEvictStrategy* strat) {
    (void)alloc; (void)strat; return 0;
}
int arix_vmem_evict_page(ArixVMemAllocator* alloc, void* addr, size_t size) {
    (void)alloc; (void)addr; (void)size; return 0;
}
void arix_vmem_set_oom_handler(ArixVMemAllocator* alloc, int (*handler)(size_t)) {
    (void)alloc; (void)handler;
}
