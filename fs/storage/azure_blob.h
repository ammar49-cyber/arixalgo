#ifndef SNEPPX_AZURE_BLOB_H
#define SNEPPX_AZURE_BLOB_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_azure_blob_create(const char* connection_string, const char* container_name);
void SNEPPX_azure_blob_destroy(void* client);
int SNEPPX_azure_blob_upload(void* client, const char* blob_name, const unsigned char* data, size_t len, const char* content_type);
int SNEPPX_azure_blob_download(void* client, const char* blob_name, unsigned char** data, size_t* len);
int SNEPPX_azure_blob_delete(void* client, const char* blob_name);
int SNEPPX_azure_blob_list(void* client, const char* prefix, char*** blobs, size_t* count);
int SNEPPX_azure_blob_exists(void* client, const char* blob_name);
int SNEPPX_azure_blob_get_url(void* client, const char* blob_name, char* url, size_t url_max);
#ifdef __cplusplus
}
#endif
#endif
