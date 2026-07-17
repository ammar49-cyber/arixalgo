#include "tiered_memory.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_tiered_create(const size_t* tier_sizes, int num_tiers, const char** tier_names) { (void)tier_sizes; (void)num_tiers; (void)tier_names; return calloc(1, 64); }
void SNEPPX_tiered_destroy(void* tm) { free(tm); }
void* SNEPPX_tiered_alloc(void* tm, size_t size, int preferred_tier) { (void)tm; (void)size; (void)preferred_tier; return NULL; }
int SNEPPX_tiered_free(void* tm, void* ptr) { (void)tm; (void)ptr; return 0; }
int SNEPPX_tiered_migrate(void* tm, void* ptr, int target_tier) { (void)tm; (void)ptr; (void)target_tier; return 0; }
int SNEPPX_tiered_get_tier(void* tm, const void* ptr) { (void)tm; (void)ptr; return 0; }
size_t SNEPPX_tiered_used(void* tm, int tier) { (void)tm; (void)tier; return 0; }
size_t SNEPPX_tiered_capacity(void* tm, int tier) { (void)tm; (void)tier; return 0; }
