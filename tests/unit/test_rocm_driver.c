#include "rocm_driver.h"
#include <stdio.h>

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

static void test_rocm_register_driver(void) {
    int ret = SNEPPX_rocm_register_driver();
    ASSERT(ret == 0, "rocm_register_driver returns 0 (stub)");
}

static void test_rocm_device_count(void) {
    int count = -1;
    int ret = SNEPPX_rocm_get_device_count(&count);
    ASSERT(ret == 0, "rocm_get_device_count returns 0");
    ASSERT(count >= 0, "device count >= 0");
}

static void test_rocm_malloc_free(void) {
    void* ptr = NULL;
    int ret = SNEPPX_rocm_mem_alloc(&ptr, 1024);
    ASSERT(ret == 0, "rocm_mem_alloc returns 0 (stub)");
    if (ptr) SNEPPX_rocm_mem_free(ptr);
}

static void test_rocm_memcpy(void) {
    float host[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    void* device = NULL;
    SNEPPX_rocm_mem_alloc(&device, 16);
    int ret = SNEPPX_rocm_mem_htod(device, host, 16);
    ASSERT(ret == 0, "memcpy to device (stub)");
    if (device) SNEPPX_rocm_mem_free(device);
}

int main(void) {
    run_test("rocm_register_driver", test_rocm_register_driver);
    run_test("rocm_device_count", test_rocm_device_count);
    run_test("rocm_malloc_free", test_rocm_malloc_free);
    run_test("rocm_memcpy", test_rocm_memcpy);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
