#include "shim_layer.h"
#include "neural_core/drivers/driver_status.h"
#include "neural_core/drivers/reference_compute.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef SNEPPX_BUILD_SHIM

/* External driver symbols — linked from the same library */
extern int SNEPPX_amd_register(void);
extern int SNEPPX_amd_get_device_count(int*);
extern void* SNEPPX_amd_create_context(int);
extern void SNEPPX_amd_destroy_context(void*);
extern int SNEPPX_amd_mem_alloc(void**, size_t);
extern int SNEPPX_amd_mem_free(void*);
extern int SNEPPX_amd_memcpy_htod(void*, const void*, size_t);
extern int SNEPPX_amd_memcpy_dtoh(void*, const void*, size_t);
extern int SNEPPX_amd_launch_kernel(const char*, void*, void**, size_t, size_t, size_t);

typedef struct {
    char backend_type[32];
    int initialized;
    int device_count;
    int (*backend_register)(void);
    int (*backend_get_device_count)(int*);
    void* (*backend_create_context)(int);
    void (*backend_destroy_context)(void*);
    int (*backend_mem_alloc)(void**, size_t);
    int (*backend_mem_free)(void*);
    int (*backend_memcpy_htod)(void*, const void*, size_t);
    int (*backend_memcpy_dtoh)(void*, const void*, size_t);
    int (*backend_launch_kernel)(const char*, void*, void**, size_t, size_t, size_t);
} ShimState;

static ShimState g_shim;

int SNEPPX_shim_init(const char* backend_type) {
    if (!backend_type) return SNEPPX_DRIVER_ERROR;
    memset(&g_shim, 0, sizeof(g_shim));
    strncpy(g_shim.backend_type, backend_type, sizeof(g_shim.backend_type) - 1);
    g_shim.backend_type[sizeof(g_shim.backend_type) - 1] = '\0';
    if (sneppx_ref_stricmp(backend_type, "amd") == 0) {
        g_shim.backend_register = SNEPPX_amd_register;
        g_shim.backend_get_device_count = SNEPPX_amd_get_device_count;
        g_shim.backend_create_context = (void* (*)(int))SNEPPX_amd_create_context;
        g_shim.backend_destroy_context = (void (*)(void*))SNEPPX_amd_destroy_context;
        g_shim.backend_mem_alloc = SNEPPX_amd_mem_alloc;
        g_shim.backend_mem_free = SNEPPX_amd_mem_free;
        g_shim.backend_memcpy_htod = SNEPPX_amd_memcpy_htod;
        g_shim.backend_memcpy_dtoh = SNEPPX_amd_memcpy_dtoh;
        g_shim.backend_launch_kernel = SNEPPX_amd_launch_kernel;
    } else {
        return SNEPPX_DRIVER_UNSUPPORTED;
    }
    if (g_shim.backend_register) {
        int ret = g_shim.backend_register();
        if (ret != SNEPPX_DRIVER_OK) return ret;
    }
    g_shim.initialized = 1;
    if (g_shim.backend_get_device_count)
        g_shim.backend_get_device_count(&g_shim.device_count);
    return SNEPPX_DRIVER_OK;
}

void SNEPPX_shim_cleanup(void) {
    memset(&g_shim, 0, sizeof(g_shim));
}

int SNEPPX_shim_get_device_count(int* count) {
    if (!g_shim.initialized) return SNEPPX_DRIVER_UNSUPPORTED;
    if (count) *count = g_shim.device_count;
    return g_shim.device_count > 0 ? SNEPPX_DRIVER_OK : SNEPPX_DRIVER_UNSUPPORTED;
}

int SNEPPX_shim_get_device_props(int dev_id, char* name, size_t name_max, int* dev_type, size_t* global_mem) {
    (void)dev_id;
    if (!g_shim.initialized) return SNEPPX_DRIVER_UNSUPPORTED;
    if (name) snprintf(name, name_max, "Shim[%s] Device %d", g_shim.backend_type, dev_id);
    if (dev_type) *dev_type = 0;
    if (global_mem) *global_mem = 4ULL * 1024 * 1024 * 1024;
    return SNEPPX_DRIVER_OK;
}

void* SNEPPX_shim_create_context(int device_id) {
    if (!g_shim.initialized || !g_shim.backend_create_context) return NULL;
    return g_shim.backend_create_context(device_id);
}

void SNEPPX_shim_destroy_context(void* ctx) {
    if (g_shim.backend_destroy_context) g_shim.backend_destroy_context(ctx);
}

int SNEPPX_shim_mem_alloc(void** dev_ptr, size_t bytes, void* ctx) {
    (void)ctx;
    if (!g_shim.initialized || !g_shim.backend_mem_alloc) return SNEPPX_DRIVER_UNSUPPORTED;
    return g_shim.backend_mem_alloc(dev_ptr, bytes);
}

int SNEPPX_shim_mem_free(void* dev_ptr, void* ctx) {
    (void)ctx;
    if (!g_shim.initialized || !g_shim.backend_mem_free) return SNEPPX_DRIVER_UNSUPPORTED;
    return g_shim.backend_mem_free(dev_ptr);
}

int SNEPPX_shim_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes, void* ctx) {
    (void)ctx;
    if (!g_shim.initialized || !g_shim.backend_memcpy_htod) return SNEPPX_DRIVER_UNSUPPORTED;
    return g_shim.backend_memcpy_htod(dev_dst, host_src, bytes);
}

int SNEPPX_shim_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes, void* ctx) {
    (void)ctx;
    if (!g_shim.initialized || !g_shim.backend_memcpy_dtoh) return SNEPPX_DRIVER_UNSUPPORTED;
    return g_shim.backend_memcpy_dtoh(host_dst, dev_src, bytes);
}

int SNEPPX_shim_launch_kernel(const char* kernel_name, void* ctx, void** args, size_t num_args, size_t global_size, size_t local_size) {
    (void)ctx;
    if (!g_shim.initialized || !g_shim.backend_launch_kernel) return SNEPPX_DRIVER_UNSUPPORTED;
    return g_shim.backend_launch_kernel(kernel_name, ctx, args, num_args, global_size, local_size);
}

int SNEPPX_shim_synchronize(void* ctx) {
    (void)ctx;
    if (!g_shim.initialized) return SNEPPX_DRIVER_UNSUPPORTED;
    return SNEPPX_DRIVER_OK;
}

#else

int SNEPPX_shim_init(const char* backend_type) { (void)backend_type; return SNEPPX_DRIVER_UNSUPPORTED; }
void SNEPPX_shim_cleanup(void) {}
int SNEPPX_shim_get_device_count(int* count) { if (count) *count = 0; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_shim_get_device_props(int dev_id, char* name, size_t name_max, int* dev_type, size_t* global_mem) { (void)dev_id; if (name) snprintf(name, name_max, "Shim Device %d", dev_id); if (dev_type) *dev_type = 0; if (global_mem) *global_mem = 4ULL*1024*1024*1024; return SNEPPX_DRIVER_OK; }
void* SNEPPX_shim_create_context(int device_id) { (void)device_id; return calloc(1, sizeof(void*)); }
void SNEPPX_shim_destroy_context(void* ctx) { free(ctx); }
int SNEPPX_shim_mem_alloc(void** dev_ptr, size_t bytes, void* ctx) { (void)dev_ptr; (void)bytes; (void)ctx; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_shim_mem_free(void* dev_ptr, void* ctx) { (void)dev_ptr; (void)ctx; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_shim_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes, void* ctx) { (void)dev_dst; (void)host_src; (void)bytes; (void)ctx; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_shim_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes, void* ctx) { (void)host_dst; (void)dev_src; (void)bytes; (void)ctx; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_shim_launch_kernel(const char* kernel_name, void* ctx, void** args, size_t num_args, size_t global_size, size_t local_size) { (void)kernel_name; (void)ctx; (void)args; (void)num_args; (void)global_size; (void)local_size; return SNEPPX_DRIVER_UNSUPPORTED; }
int SNEPPX_shim_synchronize(void* ctx) { (void)ctx; return SNEPPX_DRIVER_UNSUPPORTED; }

#endif
