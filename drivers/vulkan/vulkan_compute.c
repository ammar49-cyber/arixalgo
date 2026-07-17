#include "vulkan_compute.h"
#include <stdlib.h>

int SNEPPX_vulkan_init(void) { return 0; }
void SNEPPX_vulkan_cleanup(void) {}
int SNEPPX_vulkan_get_device_count(int* count) { if (count) *count = 0; return 0; }
int SNEPPX_vulkan_get_device_props(int dev_id, char* name, size_t name_max, size_t* global_mem) { (void)dev_id; if (name) snprintf(name, name_max, "Vulkan Device %d", dev_id); if (global_mem) *global_mem = 4ULL*1024*1024*1024; return 0; }
void* SNEPPX_vulkan_create_compute_pipeline(const char* shader_path, const char* entry_point) { (void)shader_path; (void)entry_point; return calloc(1, 8); }
void SNEPPX_vulkan_destroy_compute_pipeline(void* pipeline) { free(pipeline); }
int SNEPPX_vulkan_create_buffer(void** buf, size_t size, int usage, void* pipeline) { (void)buf; (void)size; (void)usage; (void)pipeline; return 0; }
int SNEPPX_vulkan_destroy_buffer(void* buf, void* pipeline) { (void)buf; (void)pipeline; return 0; }
int SNEPPX_vulkan_write_buffer(void* buf, const void* data, size_t offset, size_t size, void* pipeline) { (void)buf; (void)data; (void)offset; (void)size; (void)pipeline; return 0; }
int SNEPPX_vulkan_read_buffer(void* dst, const void* buf, size_t offset, size_t size, void* pipeline) { (void)dst; (void)buf; (void)offset; (void)size; (void)pipeline; return 0; }
int SNEPPX_vulkan_dispatch(void* pipeline, unsigned int group_x, unsigned int group_y, unsigned int group_z) { (void)pipeline; (void)group_x; (void)group_y; (void)group_z; return 0; }
