#include "metal_compute.h"
#include <stdlib.h>

int SNEPPX_metal_init(void) { return 0; }
void SNEPPX_metal_cleanup(void) {}
int SNEPPX_metal_get_device_count(int* count) { if (count) *count = 0; return 0; }
void* SNEPPX_metal_create_device(int dev_id) { (void)dev_id; return calloc(1, 8); }
void SNEPPX_metal_destroy_device(void* device) { free(device); }
void* SNEPPX_metal_create_command_queue(void* device) { (void)device; return calloc(1, 8); }
void SNEPPX_metal_destroy_command_queue(void* queue) { free(queue); }
void* SNEPPX_metal_create_buffer(void* device, size_t size, int storage_mode) { (void)device; (void)size; (void)storage_mode; return calloc(1, 8); }
void SNEPPX_metal_destroy_buffer(void* buf) { free(buf); }
int SNEPPX_metal_write_buffer(void* buf, const void* data, size_t offset, size_t size) { (void)buf; (void)data; (void)offset; (void)size; return 0; }
int SNEPPX_metal_read_buffer(void* dst, const void* buf, size_t offset, size_t size) { (void)dst; (void)buf; (void)offset; (void)size; return 0; }
int SNEPPX_metal_dispatch(void* queue, const char* kernel_name, void* buffers, size_t num_buffers, unsigned int grid_x, unsigned int grid_y, unsigned int grid_z) { (void)queue; (void)kernel_name; (void)buffers; (void)num_buffers; (void)grid_x; (void)grid_y; (void)grid_z; return 0; }
