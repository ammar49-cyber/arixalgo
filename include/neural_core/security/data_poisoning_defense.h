#ifndef ARIX_DATA_POISONING_DEFENSE_H
#define ARIX_DATA_POISONING_DEFENSE_H
/*
 * S5 AI Sanitizer — Data Poisoning Defense
 * Detects poisoned training samples, outlier data points, and
 * backdoor triggers in datasets.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_POISON_MAX_FEATURES 1024

typedef struct {
    double* feature_means;
    double* feature_stds;
    int feature_count;
    double outlier_threshold;
    int trained;
} ArixPoisonDetector;

int  arix_poison_detector_init(ArixPoisonDetector* pd, int feature_count);
void arix_poison_detector_destroy(ArixPoisonDetector* pd);
int  arix_poison_detector_train(ArixPoisonDetector* pd,
                                 const double* samples, int sample_count);
int  arix_poison_detector_score(ArixPoisonDetector* pd,
                                 const double* sample, int feature_count,
                                 double* outlier_score);
int  arix_poison_detector_is_outlier(ArixPoisonDetector* pd,
                                      const double* sample, int feature_count);

#ifdef __cplusplus
}
#endif
#endif
