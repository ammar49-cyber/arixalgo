#ifndef SNEPPX_S3_CLIENT_H
#define SNEPPX_S3_CLIENT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_s3_create(const char* endpoint, const char* region, const char* access_key, const char* secret_key, int use_ssl);
void SNEPPX_s3_destroy(void* client);
int SNEPPX_s3_list_buckets(void* client, char*** buckets, size_t* count);
int SNEPPX_s3_create_bucket(void* client, const char* bucket);
int SNEPPX_s3_delete_bucket(void* client, const char* bucket);
int SNEPPX_s3_put_object(void* client, const char* bucket, const char* key, const unsigned char* data, size_t len, const char* content_type);
int SNEPPX_s3_get_object(void* client, const char* bucket, const char* key, unsigned char** data, size_t* len);
int SNEPPX_s3_delete_object(void* client, const char* bucket, const char* key);
int SNEPPX_s3_list_objects(void* client, const char* bucket, const char* prefix, char*** keys, size_t* count);
int SNEPPX_s3_head_object(void* client, const char* bucket, const char* key, unsigned long long* size, char** etag, char** last_modified);
#ifdef __cplusplus
}
#endif
#endif
