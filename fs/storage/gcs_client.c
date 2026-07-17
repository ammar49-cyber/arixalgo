#include "gcs_client.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_gcs_create(const char* project_id, const char* bucket_name, const char* credentials_json) { (void)project_id; (void)bucket_name; (void)credentials_json; return NULL; }
void SNEPPX_gcs_destroy(void* client) { free(client); }
int SNEPPX_gcs_upload(void* client, const char* object_path, const unsigned char* data, size_t len, const char* content_type) { (void)client; (void)object_path; (void)data; (void)len; (void)content_type; return 0; }
int SNEPPX_gcs_download(void* client, const char* object_path, unsigned char** data, size_t* len) { (void)client; (void)object_path; (void)data; (void)len; return 0; }
int SNEPPX_gcs_delete(void* client, const char* object_path) { (void)client; (void)object_path; return 0; }
int SNEPPX_gcs_list(void* client, const char* prefix, char*** objects, size_t* count) { (void)client; (void)prefix; (void)objects; (void)count; return 0; }
int SNEPPX_gcs_exists(void* client, const char* object_path) { (void)client; (void)object_path; return 0; }
