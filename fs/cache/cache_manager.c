#include "cache_manager.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void* SNEPPX_cache_create(size_t max_size_bytes, int ttl_secs, const char* policy) { (void)max_size_bytes; (void)ttl_secs; (void)policy; return calloc(1, 64); }
void SNEPPX_cache_destroy(void* cache) { free(cache); }
int SNEPPX_cache_put(void* cache, const char* key, const unsigned char* data, size_t len) { (void)cache; (void)key; (void)data; (void)len; return 0; }
int SNEPPX_cache_get(void* cache, const char* key, unsigned char** data, size_t* len) { (void)cache; (void)key; (void)data; (void)len; return 0; }
int SNEPPX_cache_remove(void* cache, const char* key) { (void)cache; (void)key; return 0; }
int SNEPPX_cache_clear(void* cache) { (void)cache; return 0; }
int SNEPPX_cache_contains(void* cache, const char* key) { (void)cache; (void)key; return 0; }
size_t SNEPPX_cache_size(void* cache) { (void)cache; return 0; }
size_t SNEPPX_cache_count(void* cache) { (void)cache; return 0; }
double SNEPPX_cache_hit_rate(void* cache) { (void)cache; return 0.0; }
