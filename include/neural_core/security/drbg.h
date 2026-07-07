#ifndef ARIX_DRBG_H
#define ARIX_DRBG_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_DRBG_MAX_OUTPUT 65536
#define ARIX_DRBG_SEED_SIZE 48

typedef struct {
    uint8_t v[ARIX_DRBG_SEED_SIZE];
    uint8_t c[ARIX_DRBG_SEED_SIZE];
    uint64_t reseed_counter;
    int security_strength;
    int initialized;
} ArixHashDRBG;

typedef struct {
    ArixHashDRBG hb;
    int use_hmac;
} ArixDRBG;

int  arix_drbg_init(ArixDRBG* ctx, const uint8_t* entropy, size_t entropy_len, const uint8_t* nonce, size_t nonce_len);
int  arix_drbg_reseed(ArixDRBG* ctx, const uint8_t* entropy, size_t entropy_len);
int  arix_drbg_generate(ArixDRBG* ctx, uint8_t* out, size_t out_len);
void arix_drbg_destroy(ArixDRBG* ctx);
int  arix_drbg_self_test(void);

#ifdef __cplusplus
}
#endif
#endif
