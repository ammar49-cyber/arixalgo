#ifndef SNEPPX_HDF5_FORMAT_H
#define SNEPPX_HDF5_FORMAT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_hdf5_open(const char* path, const char* mode);
void SNEPPX_hdf5_close(void* file);
int SNEPPX_hdf5_create_group(void* file, const char* path);
int SNEPPX_hdf5_dataset_exists(void* file, const char* path);
int SNEPPX_hdf5_read_dataset(void* file, const char* path, void* data, size_t* dims, size_t ndim);
int SNEPPX_hdf5_write_dataset(void* file, const char* path, const void* data, const size_t* dims, size_t ndim, int dtype);
int SNEPPX_hdf5_read_attribute(void* file, const char* path, const char* attr_name, void* data, size_t* size);
int SNEPPX_hdf5_write_attribute(void* file, const char* path, const char* attr_name, const void* data, size_t size);
int SNEPPX_hdf5_list_children(void* file, const char* path, char*** children, size_t* count);
#ifdef __cplusplus
}
#endif
#endif
