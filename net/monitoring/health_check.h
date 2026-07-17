#ifndef SNEPPX_HEALTH_CHECK_H
#define SNEPPX_HEALTH_CHECK_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_health_check_create(int interval_ms, int timeout_ms, int unhealthy_threshold);
void SNEPPX_health_check_destroy(void* hc);
int SNEPPX_health_check_add_endpoint(void* hc, const char* name, const char* url, int (*check_fn)(void* ctx), void* ctx);
int SNEPPX_health_check_start(void* hc);
int SNEPPX_health_check_stop(void* hc);
int SNEPPX_health_check_get_status(void* hc, const char* name);
int SNEPPX_health_check_is_healthy(void* hc);
int SNEPPX_health_check_get_stats(void* hc, const char* name, int* total_checks, int* passed_checks, int* failed_checks, double* avg_latency);
int SNEPPX_health_check_set_on_unhealthy(void* hc, void (*cb)(void* hc, const char* name, int status));
#ifdef __cplusplus
}
#endif
#endif
