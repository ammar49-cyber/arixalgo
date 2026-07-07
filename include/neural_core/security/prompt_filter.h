#ifndef ARIX_PROMPT_FILTER_H
#define ARIX_PROMPT_FILTER_H
/*
 * S5 AI Sanitizer — Prompt Injection & Jailbreak Detection
 * Filters incoming prompts for known injection patterns, jailbreak attempts,
 * and adversarial instructions.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_MAX_PATTERNS 256
#define ARIX_PATTERN_MAX_LEN 128

typedef enum {
    ARIX_FILTER_CLEAN = 0,
    ARIX_FILTER_INJECTION = 1,
    ARIX_FILTER_JAILBREAK = 2,
    ARIX_FILTER_ADVERSARIAL = 3,
    ARIX_FILTER_SUSPICIOUS = 4,
} ArixFilterResult;

typedef struct {
    char pattern[ARIX_PATTERN_MAX_LEN];
    ArixFilterResult classification;
    int is_active;
} ArixFilterPattern;

typedef struct {
    ArixFilterPattern patterns[ARIX_MAX_PATTERNS];
    int pattern_count;
    int enabled;
    int max_token_length;
    double anomaly_threshold;
} ArixPromptFilter;

int  arix_prompt_filter_init(ArixPromptFilter* pf);
void arix_prompt_filter_destroy(ArixPromptFilter* pf);
int  arix_prompt_filter_add_pattern(ArixPromptFilter* pf, const char* pattern,
                                     ArixFilterResult classification);
ArixFilterResult arix_prompt_filter_scan(ArixPromptFilter* pf,
                                          const char* prompt, size_t len);
int  arix_prompt_filter_sanitize(ArixPromptFilter* pf,
                                  const char* prompt, size_t len,
                                  char* sanitized, size_t* sanitized_len);
void arix_prompt_filter_load_defaults(ArixPromptFilter* pf);

#ifdef __cplusplus
}
#endif
#endif
