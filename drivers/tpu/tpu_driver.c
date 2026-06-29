/*
 * TPU Driver Implementation — SKELETON
 * VERSION: v2.0
 */

#include "tpu_driver.h"
#include <stdlib.h>
#include <string.h>

int arix_tpu_register_driver(void) { return 0; }

int arix_tpu_get_device_count(int* count) { if (count) *count = 0; return 0; }

int arix_tpu_get_device_props(int dev_id, ArixTPUDeviceProps* props) {
    (void)dev_id; if (props) memset(props, 0, sizeof(*props)); return 0;
}

ArixTPUContext* arix_tpu_create_context(int device_id) {
    (void)device_id;
    ArixTPUContext* ctx = (ArixTPUContext*)calloc(1, sizeof(ArixTPUContext));
    return ctx;
}

void arix_tpu_destroy_context(ArixTPUContext* ctx) { free(ctx); }

int arix_tpu_mem_alloc(void** dev_ptr, size_t bytes, ArixTPUContext* ctx) {
    (void)ctx; if (!dev_ptr) return -1; (void)bytes; *dev_ptr = NULL; return 0;
}

int arix_tpu_mem_free(void* dev_ptr, ArixTPUContext* ctx) {
    (void)dev_ptr; (void)ctx; return 0;
}

int arix_tpu_mem_htod(void* dev_dst, const void* host_src, size_t bytes, ArixTPUContext* ctx) {
    (void)dev_dst; (void)host_src; (void)bytes; (void)ctx; return 0;
}

int arix_tpu_mem_dtoh(void* host_dst, const void* dev_src, size_t bytes, ArixTPUContext* ctx) {
    (void)host_dst; (void)dev_src; (void)bytes; (void)ctx; return 0;
}

ArixTPUExecutable* arix_tpu_compile(const char* hlo_module, size_t hlo_len, ArixTPUContext* ctx) {
    (void)hlo_module; (void)hlo_len; (void)ctx;
    return NULL;
}

void arix_tpu_executable_destroy(ArixTPUExecutable* exec) { free(exec); }

int arix_tpu_execute(ArixTPUExecutable* exec, ArixTensor** inputs, size_t num_inputs,
                     ArixTensor** outputs, size_t num_outputs, ArixTPUContext* ctx) {
    (void)exec; (void)inputs; (void)num_inputs; (void)outputs; (void)num_outputs; (void)ctx;
    return 0;
}

int arix_tpu_all_reduce(void* send_buf, void* recv_buf, size_t count,
                        int dtype, int reduce_op, ArixTPUContext* ctx) {
    (void)send_buf; (void)recv_buf; (void)count; (void)dtype; (void)reduce_op; (void)ctx;
    return 0;
}
