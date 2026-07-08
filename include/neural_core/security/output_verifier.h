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
} ArixS5Verifier;

int  arix_s5_verifier_init(ArixS5Verifier* ov);
void arix_s5_verifier_destroy(ArixS5Verifier* ov);
int  arix_s5_verifier_add_blocked_topic(ArixS5Verifier* ov, const char* topic);
int  arix_s5_verifier_check(ArixS5Verifier* ov, const char* output, size_t len);
int  arix_s5_verifier_sanitize(ArixS5Verifier* ov,
                                const char* output, size_t len,
                                char* safe_output, size_t* safe_len);

#ifdef __cplusplus
}
#endif
#endif
