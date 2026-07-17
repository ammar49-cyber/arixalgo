#ifndef SNEPPX_CACHE_MANAGER_H
#define SNEPPX_CACHE_MANAGER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_cache_create(size_t max_size_bytes, int ttl_secs, const char* policy);
void SNEPPX_cache_destroy(void* cache);
int SNEPPX_cache_put(void* cache, const char* key, const unsigned char* data, size_t len);
int SNEPPX_cache_get(void* cache, const char* key, unsigned char** data, size_t* len);
int SNEPPX_cache_remove(void* cache, const char* key);
int SNEPPX_cache_clear(void* cache);
int SNEPPX_cache_contains(void* cache, const char* key);
size_t SNEPPX_cache_size(void* cache);
size_t SNEPPX_cache_count(void* cache);
double SNEPPX_cache_hit_rate(void* cache);
#ifdef __cplusplus
}
#endif
#endif
