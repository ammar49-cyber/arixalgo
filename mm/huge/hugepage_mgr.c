#include "hugepage_mgr.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_hugepage_create(size_t total_size, size_t page_size) { (void)total_size; (void)page_size; return calloc(1, 32); }
void SNEPPX_hugepage_destroy(void* mgr) { free(mgr); }
void* SNEPPX_hugepage_alloc(void* mgr, size_t size) { (void)mgr; (void)size; return NULL; }
int SNEPPX_hugepage_free(void* mgr, void* ptr) { (void)mgr; (void)ptr; return 0; }
int SNEPPX_hugepage_promote(void* addr, size_t size) { (void)addr; (void)size; return 0; }
int SNEPPX_hugepage_demote(void* addr, size_t size) { (void)addr; (void)size; return 0; }
size_t SNEPPX_hugepage_page_size(void) { return 0; }
size_t SNEPPX_hugepage_available(void) { return 0; }
