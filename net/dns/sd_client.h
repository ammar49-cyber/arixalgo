#ifndef SNEPPX_SD_CLIENT_H
#define SNEPPX_SD_CLIENT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_sd_init(const char* service_name, const char* service_type, int port);
void SNEPPX_sd_destroy(void* sd);
int SNEPPX_sd_register(void* sd);
int SNEPPX_sd_unregister(void* sd);
int SNEPPX_sd_discover(void* sd, char*** hosts, int** ports, size_t* count, int timeout_ms);
int SNEPPX_sd_resolve(const char* name, char* host, size_t host_max, int* port);
int SNEPPX_sd_set_txt_record(void* sd, const char* key, const char* value);
int SNEPPX_sd_get_txt_record(void* sd, const char* key, char* value, size_t value_max);
#ifdef __cplusplus
}
#endif
#endif
