#include "integrity_monitor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_passed = 0;
static int tests_failed = 0;
static int g_event_count = 0;

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

static void event_callback(const ArixMonitorEvent* event) {
    (void)event;
    g_event_count++;
}

static void test_init_shutdown(void) {
    int ret = arix_monitor_init();
    ASSERT(ret == 0, "monitor init");
    arix_monitor_shutdown();
}

static void test_start_stop(void) {
    arix_monitor_init();
    int ret = arix_monitor_start(100);
    ASSERT(ret == 0, "monitor start");
    ret = arix_monitor_stop();
    ASSERT(ret == 0, "monitor stop");
    arix_monitor_shutdown();
}

static void test_region_registration(void) {
    arix_monitor_init();
    int data[] = {1, 2, 3, 4, 5};
    int ret = arix_monitor_register_region("test_data", data, sizeof(data));
    ASSERT(ret == 0, "region registered");

    ret = arix_monitor_verify_region("test_data");
    ASSERT(ret == 0, "region verified unchanged");

    ret = arix_monitor_unregister_region("test_data");
    ASSERT(ret == 0, "region unregistered");

    ret = arix_monitor_verify_region("test_data");
    ASSERT(ret == -1, "unregistered region returns -1");
    arix_monitor_shutdown();
}

static void test_verify_all(void) {
    arix_monitor_init();
    int data[] = {10, 20, 30};
    arix_monitor_register_region("verify_data", data, sizeof(data));
    int violations = arix_monitor_verify_all();
    ASSERT(violations == 0, "verify all returns 0 violations");
    arix_monitor_unregister_region("verify_data");
    arix_monitor_shutdown();
}

static void test_canary(void) {
    arix_monitor_init();
    int ok = arix_monitor_check_canary();
    ASSERT(ok != 0, "canary check passes initially");
    arix_monitor_refresh_canary();
    ok = arix_monitor_check_canary();
    ASSERT(ok != 0, "canary check passes after refresh");
    arix_monitor_shutdown();
}

static void test_callback(void) {
    arix_monitor_init();
    arix_monitor_set_callback(event_callback);
    g_event_count = 0;

    /* Register a small region and modify it to trigger an event */
    char data[] = "original data";
    arix_monitor_register_region("callback_data", data, sizeof(data));

    /* Modify the data */
    data[0] = 'X';

    int violations = arix_monitor_verify_all();
    ASSERT(violations > 0, "violations detected after modification");
    ASSERT(g_event_count > 0, "callback fired");
    arix_monitor_unregister_region("callback_data");
    arix_monitor_shutdown();
}

int main(void) {
    run_test("init_shutdown", test_init_shutdown);
    run_test("start_stop", test_start_stop);
    run_test("region_registration", test_region_registration);
    run_test("verify_all", test_verify_all);
    run_test("canary", test_canary);
    run_test("callback", test_callback);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
