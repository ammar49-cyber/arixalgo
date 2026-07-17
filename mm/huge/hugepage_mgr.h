#ifndef SNEPPX_HUGEPAGE_MGR_H
#define SNEPPX_HUGEPAGE_MGR_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_hugepage_create(size_t total_size, size_t page_size);
void SNEPPX_hugepage_destroy(void* mgr);
void* SNEPPX_hugepage_alloc(void* mgr, size_t size);
int SNEPPX_hugepage_free(void* mgr, void* ptr);
int SNEPPX_hugepage_promote(void* addr, size_t size);
int SNEPPX_hugepage_demote(void* addr, size_t size);
size_t SNEPPX_hugepage_page_size(void);
size_t SNEPPX_hugepage_available(void);
#ifdef __cplusplus
}
#endif
#endif
