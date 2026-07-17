#ifndef SNEPPX_CIRCUIT_BREAKER_H
#define SNEPPX_CIRCUIT_BREAKER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { void* impl; } SNEPPXCircuitBreaker;
typedef enum { SNEPPX_CB_CLOSED, SNEPPX_CB_OPEN, SNEPPX_CB_HALF_OPEN } SNEPPXCBState;
SNEPPXCircuitBreaker* SNEPPX_cb_create(const char* name, int failure_threshold, int recovery_timeout_ms, int half_open_max_requests);
void SNEPPX_cb_destroy(SNEPPXCircuitBreaker* cb);
int SNEPPX_cb_call(SNEPPXCircuitBreaker* cb, int (*fn)(void*), void* arg);
SNEPPXCBState SNEPPX_cb_state(SNEPPXCircuitBreaker* cb);
void SNEPPX_cb_reset(SNEPPXCircuitBreaker* cb);
int SNEPPX_cb_failure_count(SNEPPXCircuitBreaker* cb);
#ifdef __cplusplus
}
#endif
#endif
