#ifndef SNEPPX_METAL_COMPUTE_H
#define SNEPPX_METAL_COMPUTE_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int  SNEPPX_metal_init(void);
void SNEPPX_metal_cleanup(void);
int  SNEPPX_metal_get_device_count(int* count);
void* SNEPPX_metal_create_device(int dev_id);
void  SNEPPX_metal_destroy_device(void* device);
void* SNEPPX_metal_create_command_queue(void* device);
void  SNEPPX_metal_destroy_command_queue(void* queue);
void* SNEPPX_metal_create_buffer(void* device, size_t size, int storage_mode);
void  SNEPPX_metal_destroy_buffer(void* buf);
int   SNEPPX_metal_write_buffer(void* buf, const void* data, size_t offset, size_t size);
int   SNEPPX_metal_read_buffer(void* dst, const void* buf, size_t offset, size_t size);
int   SNEPPX_metal_dispatch(void* queue, const char* kernel_name, void* buffers, size_t num_buffers, unsigned int grid_x, unsigned int grid_y, unsigned int grid_z);
#ifdef __cplusplus
}
#endif
#endif
