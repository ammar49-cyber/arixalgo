#ifndef ARIX_BIGNUM_H
#define ARIX_BIGNUM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_BN_MAX_WORDS 128
#define ARIX_BN_WORD uint64_t
#define ARIX_BN_HALF_WORD uint32_t

typedef struct {
    ARIX_BN_WORD words[ARIX_BN_MAX_WORDS];
    int used;
    int sign;
} ArixBigNum;

void arix_bn_init(ArixBigNum* bn);
void arix_bn_zero(ArixBigNum* bn);
int  arix_bn_set_word(ArixBigNum* bn, ARIX_BN_WORD val);
int  arix_bn_set_array(ArixBigNum* bn, const uint8_t* bytes, size_t len);
int  arix_bn_from_hex(ArixBigNum* bn, const char* hex);
void arix_bn_to_array(const ArixBigNum* bn, uint8_t* out, size_t* out_len);

int  arix_bn_copy(ArixBigNum* dst, const ArixBigNum* src);
int  arix_bn_is_zero(const ArixBigNum* bn);
int  arix_bn_is_one(const ArixBigNum* bn);
int  arix_bn_cmp(const ArixBigNum* a, const ArixBigNum* b);
int  arix_bn_cmp_word(const ArixBigNum* a, ARIX_BN_WORD b);

int  arix_bn_add(ArixBigNum* r, const ArixBigNum* a, const ArixBigNum* b);
int  arix_bn_sub(ArixBigNum* r, const ArixBigNum* a, const ArixBigNum* b);
int  arix_bn_mul(ArixBigNum* r, const ArixBigNum* a, const ArixBigNum* b);
int  arix_bn_div(ArixBigNum* q, ArixBigNum* rem, const ArixBigNum* a, const ArixBigNum* b);
int  arix_bn_mod(ArixBigNum* r, const ArixBigNum* a, const ArixBigNum* m);
int  arix_bn_exp_mod(ArixBigNum* r, const ArixBigNum* base, const ArixBigNum* exp, const ArixBigNum* mod);

int  arix_bn_gcd(ArixBigNum* r, const ArixBigNum* a, const ArixBigNum* b);
int  arix_bn_inv_mod(ArixBigNum* r, const ArixBigNum* a, const ArixBigNum* m);
int  arix_bn_is_prime(const ArixBigNum* bn);
void arix_bn_print(const ArixBigNum* bn);

#ifdef __cplusplus
}
#endif
#endif
