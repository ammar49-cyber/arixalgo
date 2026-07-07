#include "cuda_driver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s (%s)\n", msg, #cond); \
        tests_failed++; \
        return; \
    } \
} while(0)

static void run_test(const char* name, void (*test_fn)(void)) {
    printf("Running %s... ", name);
    fflush(stdout);
    test_fn();
    printf("PASS\n");
    tests_passed++;
}

static void test_register_driver(void) {
    int ret = arix_cuda_register_driver();
    ASSERT(ret == 0, "driver registration");
}

static void test_get_device_count(void) {
    int count = -1;
    int ret = arix_cuda_get_device_count(&count);
    ASSERT(ret == 0, "get device count returns 0");
    ASSERT(count >= 0, "device count is non-negative");
}

static void test_get_device_props(void) {
    ArixCUDADeviceProps props;
    memset(&props, 0, sizeof(props));
    int ret = arix_cuda_get_device_props(0, &props);
    ASSERT(ret == 0, "get device props returns 0");
}

static void test_set_get_device(void) {
    int ret = arix_cuda_set_device(0);
    ASSERT(ret == 0, "set device 0");
    int dev = -1;
    ret = arix_cuda_get_device(&dev);
    ASSERT(ret == 0, "get device");
}

static void test_context_create_destroy(void) {
    ArixCUDAContext* ctx = arix_cuda_create_context(0);
    /* May return NULL if CUDA not available - that's OK */
    if (ctx) {
        ASSERT(ctx->device_id == 0, "context device id");
        int err = arix_cuda_context_error(ctx);
        ASSERT(err == 0, "no context error");
        arix_cuda_destroy_context(ctx);
    }
}

static void test_stream_create_destroy(void) {
    ArixCUDAStream* stream = NULL;
    int ret = arix_cuda_stream_create(&stream, 0);
    ASSERT(ret == 0, "stream create");
    if (stream) {
        arix_cuda_stream_destroy(stream);
    }
}

static void test_mem_alloc_free(void) {
    void* ptr = NULL;
    int ret = arix_cuda_mem_alloc(&ptr, 1024);
    ASSERT(ret == 0, "mem alloc");
    if (ptr) {
        ret = arix_cuda_mem_free(ptr);
        ASSERT(ret == 0, "mem free");
    }
}

static void test_mem_htod_dtoh(void) {
    float host_src[] = {1.0f, 2.0f, 3.0f};
    void* dev = NULL;
    arix_cuda_mem_alloc(&dev, sizeof(host_src));
    if (dev) {
        int ret = arix_cuda_mem_htod(dev, host_src, sizeof(host_src));
        ASSERT(ret == 0, "htod");
        float host_dst[3] = {0};
        ret = arix_cuda_mem_dtoh(host_dst, dev, sizeof(host_src));
        ASSERT(ret == 0, "dtoh");
        arix_cuda_mem_free(dev);
    }
}

static void test_event_create_destroy(void) {
    ArixCUDAEvent* event = NULL;
    int ret = arix_cuda_event_create(&event);
    ASSERT(ret == 0, "event create");
    if (event) {
        arix_cuda_event_destroy(event);
    }
}

static void test_error_string(void) {
    const char* err = arix_cuda_error_string(0);
    ASSERT(err != NULL, "error string non-null");
}

int main(void) {
    run_test("register_driver", test_register_driver);
    run_test("get_device_count", test_get_device_count);
    run_test("get_device_props", test_get_device_props);
    run_test("set_get_device", test_set_get_device);
    run_test("context_create_destroy", test_context_create_destroy);
    run_test("stream_create_destroy", test_stream_create_destroy);
    run_test("mem_alloc_free", test_mem_alloc_free);
    run_test("mem_htod_dtoh", test_mem_htod_dtoh);
    run_test("event_create_destroy", test_event_create_destroy);
    run_test("error_string", test_error_string);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
