#ifndef SNEPPX_TIERED_MEMORY_H
#define SNEPPX_TIERED_MEMORY_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_tiered_create(const size_t* tier_sizes, int num_tiers, const char** tier_names);
void SNEPPX_tiered_destroy(void* tm);
void* SNEPPX_tiered_alloc(void* tm, size_t size, int preferred_tier);
int SNEPPX_tiered_free(void* tm, void* ptr);
int SNEPPX_tiered_migrate(void* tm, void* ptr, int target_tier);
int SNEPPX_tiered_get_tier(void* tm, const void* ptr);
size_t SNEPPX_tiered_used(void* tm, int tier);
size_t SNEPPX_tiered_capacity(void* tm, int tier);
#ifdef __cplusplus
}
#endif
#endif
