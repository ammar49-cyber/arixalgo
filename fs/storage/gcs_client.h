#ifndef SNEPPX_GCS_CLIENT_H
#define SNEPPX_GCS_CLIENT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_gcs_create(const char* project_id, const char* bucket_name, const char* credentials_json);
void SNEPPX_gcs_destroy(void* client);
int SNEPPX_gcs_upload(void* client, const char* object_path, const unsigned char* data, size_t len, const char* content_type);
int SNEPPX_gcs_download(void* client, const char* object_path, unsigned char** data, size_t* len);
int SNEPPX_gcs_delete(void* client, const char* object_path);
int SNEPPX_gcs_list(void* client, const char* prefix, char*** objects, size_t* count);
int SNEPPX_gcs_exists(void* client, const char* object_path);
#ifdef __cplusplus
}
#endif
#endif
