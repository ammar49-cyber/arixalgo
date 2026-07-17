#include "health_check.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void* SNEPPX_health_check_create(int interval_ms, int timeout_ms, int unhealthy_threshold) { (void)interval_ms; (void)timeout_ms; (void)unhealthy_threshold; return calloc(1, 32); }
void SNEPPX_health_check_destroy(void* hc) { free(hc); }
int SNEPPX_health_check_add_endpoint(void* hc, const char* name, const char* url, int (*check_fn)(void* ctx), void* ctx) { (void)hc; (void)name; (void)url; (void)check_fn; (void)ctx; return 0; }
int SNEPPX_health_check_start(void* hc) { (void)hc; return 0; }
int SNEPPX_health_check_stop(void* hc) { (void)hc; return 0; }
int SNEPPX_health_check_get_status(void* hc, const char* name) { (void)hc; (void)name; return 1; }
int SNEPPX_health_check_is_healthy(void* hc) { (void)hc; return 1; }
int SNEPPX_health_check_get_stats(void* hc, const char* name, int* total_checks, int* passed_checks, int* failed_checks, double* avg_latency) { (void)hc; (void)name; (void)total_checks; (void)passed_checks; (void)failed_checks; (void)avg_latency; return 0; }
int SNEPPX_health_check_set_on_unhealthy(void* hc, void (*cb)(void* hc, const char* name, int status)) { (void)hc; (void)cb; return 0; }
