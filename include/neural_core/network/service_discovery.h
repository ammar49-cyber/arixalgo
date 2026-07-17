#ifndef SNEPPX_SERVICE_DISCOVERY_H
#define SNEPPX_SERVICE_DISCOVERY_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { SNEPPX_SD_DNS, SNEPPX_SD_CONSUL, SNEPPX_SD_ETCD, SNEPPX_SD_K8S } SNEPPXSDProvider;
typedef struct { char name[128]; char host[256]; int port; char** tags; size_t num_tags; uint64_t last_seen; } SNEPPXSDInstance;
void* SNEPPX_sd_init(SNEPPXSDProvider provider, const char* service_name, int port);
void SNEPPX_sd_destroy(void* sd);
int SNEPPX_sd_register(void* sd, int ttl_secs);
int SNEPPX_sd_unregister(void* sd);
int SNEPPX_sd_discover(void* sd, SNEPPXSDInstance** instances, size_t* count);
void SNEPPX_sd_instances_free(SNEPPXSDInstance* instances, size_t count);
#ifdef __cplusplus
}
#endif
#endif
