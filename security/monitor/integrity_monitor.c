/*
 * Integrity Monitor Implementation — SKELETON
 * VERSION: v3.0
 */

#include "integrity_monitor.h"
#include <stdlib.h>
#include <string.h>

static ArixMonitorCallback g_callback = NULL;

int arix_monitor_init(void) { return 0; }
void arix_monitor_shutdown(void) { g_callback = NULL; }
int arix_monitor_start(uint64_t interval_ms) { (void)interval_ms; return 0; }
int arix_monitor_stop(void) { return 0; }
int arix_monitor_register_region(const char* name, const void* addr, size_t size) {
    (void)name; (void)addr; (void)size; return 0;
}
int arix_monitor_unregister_region(const char* name) { (void)name; return 0; }
int arix_monitor_verify_all(void) { return 0; }
int arix_monitor_verify_region(const char* name) { (void)name; return 0; }
int arix_monitor_check_canary(void) { return 0; }
void arix_monitor_refresh_canary(void) {}
void arix_monitor_set_callback(ArixMonitorCallback cb) { g_callback = cb; }
