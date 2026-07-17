#ifndef SNEPPX_LOAD_BALANCER_H
#define SNEPPX_LOAD_BALANCER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { void* impl; } SNEPPXLoadBalancer;
typedef enum { SNEPPX_LB_ROUND_ROBIN, SNEPPX_LB_LEAST_CONN, SNEPPX_LB_RANDOM, SNEPPX_LB_WEIGHTED } SNEPPXLBStrategy;
SNEPPXLoadBalancer* SNEPPX_lb_create(SNEPPXLBStrategy strategy);
void SNEPPX_lb_destroy(SNEPPXLoadBalancer* lb);
int SNEPPX_lb_add_backend(SNEPPXLoadBalancer* lb, const char* host, int port, int weight);
int SNEPPX_lb_remove_backend(SNEPPXLoadBalancer* lb, const char* host, int port);
int SNEPPX_lb_next_backend(SNEPPXLoadBalancer* lb, char* host, size_t host_max, int* port);
int SNEPPX_lb_mark_unhealthy(SNEPPXLoadBalancer* lb, const char* host, int port);
int SNEPPX_lb_mark_healthy(SNEPPXLoadBalancer* lb, const char* host, int port);
#ifdef __cplusplus
}
#endif
#endif
