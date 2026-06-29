/*
 * ROCm Driver Implementation — SKELETON
 *
 * Stub implementations matching rocm_driver.h declarations.
 * VERSION: v1.0
 */

#include "rocm_driver.h"
#include <stdlib.h>
#include <string.h>

int arix_rocm_register_driver(void) { return 0; }

int arix_rocm_get_device_count(int* count) { if (count) *count = 0; return 0; }

int arix_rocm_get_device_props(int dev_id, ArixROCmDeviceProps* props) {
    (void)dev_id; if (props) memset(props, 0, sizeof(*props)); return 0;
}

int arix_rocm_set_device(int dev_id) { (void)dev_id; return 0; }

ArixROCmContext* arix_rocm_create_context(int device_id) {
    (void)device_id;
    ArixROCmContext* ctx = (ArixROCmContext*)calloc(1, sizeof(ArixROCmContext));
    return ctx;
}

void arix_rocm_destroy_context(ArixROCmContext* ctx) { free(ctx); }

int arix_rocm_stream_create(ArixROCmStream** stream) {
    if (!stream) return -1;
    *stream = (ArixROCmStream*)calloc(1, sizeof(ArixROCmStream));
    return *stream ? 0 : -1;
}

void arix_rocm_stream_destroy(ArixROCmStream* stream) { free(stream); }

int arix_rocm_stream_synchronize(ArixROCmStream* stream) { (void)stream; return 0; }

int arix_rocm_event_create(ArixROCmEvent** event) {
    if (!event) return -1;
    *event = (ArixROCmEvent*)calloc(1, sizeof(ArixROCmEvent));
    return *event ? 0 : -1;
}

void arix_rocm_event_destroy(ArixROCmEvent* event) { free(event); }

int arix_rocm_event_record(ArixROCmEvent* event, ArixROCmStream* stream) {
    (void)event; (void)stream; return 0;
}

int arix_rocm_event_synchronize(ArixROCmEvent* event) { (void)event; return 0; }

int arix_rocm_mem_alloc(void** dev_ptr, size_t bytes) {
    if (!dev_ptr) return -1; (void)bytes; *dev_ptr = NULL; return 0;
}

int arix_rocm_mem_free(void* dev_ptr) { (void)dev_ptr; return 0; }

int arix_rocm_mem_htod(void* dev_dst, const void* host_src, size_t bytes) {
    (void)dev_dst; (void)host_src; (void)bytes; return 0;
}

int arix_rocm_mem_dtoh(void* host_dst, const void* dev_src, size_t bytes) {
    (void)host_dst; (void)dev_src; (void)bytes; return 0;
}

int arix_rocm_mem_dtod(void* dev_dst, const void* dev_src, size_t bytes) {
    (void)dev_dst; (void)dev_src; (void)bytes; return 0;
}

int arix_rocm_launch_kernel(const ArixROCmKernelLaunch* launch) {
    (void)launch; return 0;
}

int arix_rocm_launch_kernel_async(const ArixROCmKernelLaunch* launch) {
    (void)launch; return 0;
}
