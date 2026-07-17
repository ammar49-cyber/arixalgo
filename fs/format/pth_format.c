#include "pth_format.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_pth_load(const char* path, const char* device) { (void)path; (void)device; return NULL; }
void SNEPPX_pth_destroy(void* state) { free(state); }
void* SNEPPX_pth_get_tensor(void* state, const char* key) { (void)state; (void)key; return NULL; }
int SNEPPX_pth_get_keys(void* state, char*** keys, size_t* count) { (void)state; (void)keys; (void)count; return 0; }
int SNEPPX_pth_set_tensor(void* state, const char* key, const void* data, const size_t* shape, size_t ndim, int dtype) { (void)state; (void)key; (void)data; (void)shape; (void)ndim; (void)dtype; return 0; }
int SNEPPX_pth_save(void* state, const char* path) { (void)state; (void)path; return 0; }
