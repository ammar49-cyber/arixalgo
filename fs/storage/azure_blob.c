#include "azure_blob.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_azure_blob_create(const char* connection_string, const char* container_name) { (void)connection_string; (void)container_name; return NULL; }
void SNEPPX_azure_blob_destroy(void* client) { free(client); }
int SNEPPX_azure_blob_upload(void* client, const char* blob_name, const unsigned char* data, size_t len, const char* content_type) { (void)client; (void)blob_name; (void)data; (void)len; (void)content_type; return 0; }
int SNEPPX_azure_blob_download(void* client, const char* blob_name, unsigned char** data, size_t* len) { (void)client; (void)blob_name; (void)data; (void)len; return 0; }
int SNEPPX_azure_blob_delete(void* client, const char* blob_name) { (void)client; (void)blob_name; return 0; }
int SNEPPX_azure_blob_list(void* client, const char* prefix, char*** blobs, size_t* count) { (void)client; (void)prefix; (void)blobs; (void)count; return 0; }
int SNEPPX_azure_blob_exists(void* client, const char* blob_name) { (void)client; (void)blob_name; return 0; }
int SNEPPX_azure_blob_get_url(void* client, const char* blob_name, char* url, size_t url_max) { (void)client; (void)blob_name; (void)url; (void)url_max; return 0; }
