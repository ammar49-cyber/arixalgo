#include "hdf5_format.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_hdf5_open(const char* path, const char* mode) { (void)path; (void)mode; return NULL; }
void SNEPPX_hdf5_close(void* file) { free(file); }
int SNEPPX_hdf5_create_group(void* file, const char* path) { (void)file; (void)path; return 0; }
int SNEPPX_hdf5_dataset_exists(void* file, const char* path) { (void)file; (void)path; return 0; }
int SNEPPX_hdf5_read_dataset(void* file, const char* path, void* data, size_t* dims, size_t ndim) { (void)file; (void)path; (void)data; (void)dims; (void)ndim; return 0; }
int SNEPPX_hdf5_write_dataset(void* file, const char* path, const void* data, const size_t* dims, size_t ndim, int dtype) { (void)file; (void)path; (void)data; (void)dims; (void)ndim; (void)dtype; return 0; }
int SNEPPX_hdf5_read_attribute(void* file, const char* path, const char* attr_name, void* data, size_t* size) { (void)file; (void)path; (void)attr_name; (void)data; (void)size; return 0; }
int SNEPPX_hdf5_write_attribute(void* file, const char* path, const char* attr_name, const void* data, size_t size) { (void)file; (void)path; (void)attr_name; (void)data; (void)size; return 0; }
int SNEPPX_hdf5_list_children(void* file, const char* path, char*** children, size_t* count) { (void)file; (void)path; (void)children; (void)count; return 0; }
