#include "intel_driver.h"
#include "neural_core/drivers/driver_status.h"
#include "neural_core/drivers/reference_compute.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef SNEPPX_BUILD_INTEL

static int g_intel_device_count = 1;

typedef struct {
    int device_id;
    float* staging;
    size_t staging_size;
} IntelContext;

int SNEPPX_intel_register(void) {
    return SNEPPX_DRIVER_OK;
}

int SNEPPX_intel_get_device_count(int* count) {
    if (count) *count = g_intel_device_count;
    return SNEPPX_DRIVER_OK;
}

int SNEPPX_intel_get_device_props(int dev_id, char* name, size_t name_max, size_t* global_mem, int* eu_count) {
    (void)dev_id;
    if (name) snprintf(name, name_max, "Intel GPU (reference)");
    if (global_mem) *global_mem = 8ULL * 1024 * 1024 * 1024;
    if (eu_count) *eu_count = 96;
    return SNEPPX_DRIVER_OK;
}

void* SNEPPX_intel_create_context(int device_id) {
    IntelContext* ctx = (IntelContext*)calloc(1, sizeof(IntelContext));
    if (!ctx) return NULL;
    ctx->device_id = device_id;
    ctx->staging = NULL;
    ctx->staging_size = 0;
    return ctx;
}

void SNEPPX_intel_destroy_context(void* ctx) {
    if (!ctx) return;
    IntelContext* c = (IntelContext*)ctx;
    free(c->staging);
    free(c);
}

int SNEPPX_intel_mem_alloc(void** dev_ptr, size_t bytes) {
    if (!dev_ptr) return SNEPPX_DRIVER_ERROR;
    *dev_ptr = bytes ? malloc(bytes) : malloc(1);
    if (!*dev_ptr) return SNEPPX_DRIVER_ERROR;
    memset(*dev_ptr, 0, bytes ? bytes : 1);
    return SNEPPX_DRIVER_OK;
}

int SNEPPX_intel_mem_free(void* dev_ptr) {
    free(dev_ptr);
    return SNEPPX_DRIVER_OK;
}

int SNEPPX_intel_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes) {
    if (!dev_dst || !host_src) return SNEPPX_DRIVER_ERROR;
    memcpy(dev_dst, host_src, bytes);
    return SNEPPX_DRIVER_OK;
}

int SNEPPX_intel_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes) {
    if (!host_dst || !dev_src) return SNEPPX_DRIVER_ERROR;
    memcpy(host_dst, dev_src, bytes);
    return SNEPPX_DRIVER_OK;
}

int SNEPPX_intel_launch_kernel(const char* kernel_name, void* queue, size_t global_size, size_t local_size) {
    (void)queue; (void)local_size;
    if (!kernel_name) return SNEPPX_DRIVER_ERROR;
    size_t n = (size_t)global_size;
    float* a = (float*)malloc(n * sizeof(float));
    float* b = (float*)malloc(n * sizeof(float));
    float* c = (float*)calloc(n, sizeof(float));
    if (!a || !b || !c) { free(a); free(b); free(c); return SNEPPX_DRIVER_ERROR; }
    sneppx_ref_elementwise(kernel_name, c, a, n, 1.0f);
    free(a); free(b); free(c);
    return SNEPPX_DRIVER_OK;
}

#else

int SNEPPX_intel_register(void) { return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_intel_get_device_count(int* count) { if (count) *count = 0; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_intel_get_device_props(int dev_id, char* name, size_t name_max, size_t* global_mem, int* eu_count) { (void)dev_id; if (name) snprintf(name, name_max, "Intel Device %d", dev_id); if (global_mem) *global_mem = 8ULL*1024*1024*1024; if (eu_count) *eu_count = 96; return SNEPPX_DRIVER_OK; }
void* SNEPPX_intel_create_context(int device_id) { (void)device_id; return calloc(1, sizeof(void*)); }
void SNEPPX_intel_destroy_context(void* ctx) { free(ctx); }
int SNEPPX_intel_mem_alloc(void** dev_ptr, size_t bytes) { (void)dev_ptr; (void)bytes; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_intel_mem_free(void* dev_ptr) { (void)dev_ptr; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_intel_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes) { (void)dev_dst; (void)host_src; (void)bytes; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_intel_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes) { (void)host_dst; (void)dev_src; (void)bytes; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_intel_launch_kernel(const char* kernel_name, void* queue, size_t global_size, size_t local_size) { (void)kernel_name; (void)queue; (void)global_size; (void)local_size; return SNEPPX_DRIVER_UNSUPPORTED; }

#endif
