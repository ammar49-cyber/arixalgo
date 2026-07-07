#ifndef ARIX_OUTPUT_VERIFIER_H
#define ARIX_OUTPUT_VERIFIER_H
/*
 * S5 AI Sanitizer — Output Verification
 * Validates AI outputs for toxicity, bias, factual consistency,
 * and policy compliance before delivery.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_MAX_TOPIC_BLOCKLIST 128
#define ARIX_TOPIC_MAX_LEN 64

typedef struct {
    char topic[ARIX_TOPIC_MAX_LEN];
    int is_blocked;
} ArixBlockedTopic;

typedef struct {
    ArixBlockedTopic topics[ARIX_MAX_TOPIC_BLOCKLIST];
    int topic_count;
    double toxicity_threshold;
    double bias_threshold;
    int check_factual_consistency;
    int max_output_length;
} ArixOutputVerifier;

int  arix_output_verifier_init(ArixOutputVerifier* ov);
void arix_output_verifier_destroy(ArixOutputVerifier* ov);
int  arix_output_verifier_add_blocked_topic(ArixOutputVerifier* ov, const char* topic);
int  arix_output_verifier_check(ArixOutputVerifier* ov, const char* output, size_t len);
int  arix_output_verifier_sanitize(ArixOutputVerifier* ov,
                                    const char* output, size_t len,
                                    char* safe_output, size_t* safe_len);

#ifdef __cplusplus
}
#endif
#endif
