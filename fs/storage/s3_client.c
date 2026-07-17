#include "s3_client.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_s3_create(const char* endpoint, const char* region, const char* access_key, const char* secret_key, int use_ssl) { (void)endpoint; (void)region; (void)access_key; (void)secret_key; (void)use_ssl; return NULL; }
void SNEPPX_s3_destroy(void* client) { free(client); }
int SNEPPX_s3_list_buckets(void* client, char*** buckets, size_t* count) { (void)client; (void)buckets; (void)count; return 0; }
int SNEPPX_s3_create_bucket(void* client, const char* bucket) { (void)client; (void)bucket; return 0; }
int SNEPPX_s3_delete_bucket(void* client, const char* bucket) { (void)client; (void)bucket; return 0; }
int SNEPPX_s3_put_object(void* client, const char* bucket, const char* key, const unsigned char* data, size_t len, const char* content_type) { (void)client; (void)bucket; (void)key; (void)data; (void)len; (void)content_type; return 0; }
int SNEPPX_s3_get_object(void* client, const char* bucket, const char* key, unsigned char** data, size_t* len) { (void)client; (void)bucket; (void)key; (void)data; (void)len; return 0; }
int SNEPPX_s3_delete_object(void* client, const char* bucket, const char* key) { (void)client; (void)bucket; (void)key; return 0; }
int SNEPPX_s3_list_objects(void* client, const char* bucket, const char* prefix, char*** keys, size_t* count) { (void)client; (void)bucket; (void)prefix; (void)keys; (void)count; return 0; }
int SNEPPX_s3_head_object(void* client, const char* bucket, const char* key, unsigned long long* size, char** etag, char** last_modified) { (void)client; (void)bucket; (void)key; (void)size; (void)etag; (void)last_modified; return 0; }
