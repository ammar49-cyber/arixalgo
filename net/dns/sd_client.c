#include "sd_client.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_sd_init(const char* service_name, const char* service_type, int port) { (void)service_name; (void)service_type; (void)port; return NULL; }
void SNEPPX_sd_destroy(void* sd) { free(sd); }
int SNEPPX_sd_register(void* sd) { (void)sd; return 0; }
int SNEPPX_sd_unregister(void* sd) { (void)sd; return 0; }
int SNEPPX_sd_discover(void* sd, char*** hosts, int** ports, size_t* count, int timeout_ms) { (void)sd; (void)hosts; (void)ports; (void)count; (void)timeout_ms; return 0; }
int SNEPPX_sd_resolve(const char* name, char* host, size_t host_max, int* port) { (void)name; (void)host; (void)host_max; (void)port; return 0; }
int SNEPPX_sd_set_txt_record(void* sd, const char* key, const char* value) { (void)sd; (void)key; (void)value; return 0; }
int SNEPPX_sd_get_txt_record(void* sd, const char* key, char* value, size_t value_max) { (void)sd; (void)key; (void)value; (void)value_max; return 0; }
