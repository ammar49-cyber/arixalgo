#ifndef ARIX_SIPHASH_H
#define ARIX_SIPHASH_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_SIPHASH_KEY_SIZE 16
#define ARIX_SIPHASH_OUT_SIZE 8

typedef struct {
    uint64_t v0, v1, v2, v3;
    uint64_t k0, k1;
    int c_rounds;
    int d_rounds;
} ArixSipHash;

void arix_siphash_init(ArixSipHash* sh, const uint8_t key[ARIX_SIPHASH_KEY_SIZE]);
void arix_siphash_update(ArixSipHash* sh, const uint8_t* data, size_t len);
uint64_t arix_siphash_finalize(ArixSipHash* sh);
uint64_t arix_siphash(const uint8_t key[ARIX_SIPHASH_KEY_SIZE], const uint8_t* data, size_t len);

void arix_siphash_24_init(ArixSipHash* sh, const uint8_t key[ARIX_SIPHASH_KEY_SIZE]);
uint64_t arix_siphash_24(const uint8_t key[ARIX_SIPHASH_KEY_SIZE], const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
#endif
