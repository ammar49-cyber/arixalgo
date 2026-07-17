#include "gradient_optimization_suite.h"
#include <stdlib.h>
#include <string.h>

struct SNEPPXGradientOptimizer {
    int dummy;
};

struct SNEPPXLRScher {
    int dummy;
};

SNEPPXGradientOptimizer* SNEPPX_grad_opt_create(SNEPPXOptimizerType type, double lr, double weight_decay) {
    (void)type; (void)lr; (void)weight_decay;
    return (SNEPPXGradientOptimizer*)calloc(1, sizeof(SNEPPXGradientOptimizer));
}

void SNEPPX_grad_opt_destroy(SNEPPXGradientOptimizer* opt) {
    free(opt);
}

int SNEPPX_grad_opt_step(SNEPPXGradientOptimizer* opt, SNEPPXTensor** params, SNEPPXTensor** grads, size_t num_params) {
    (void)opt; (void)params; (void)grads; (void)num_params;
    return 0;
}

int SNEPPX_grad_opt_zero_grad(SNEPPXGradientOptimizer* opt, SNEPPXTensor** params, size_t num_params) {
    (void)opt; (void)params; (void)num_params;
    return 0;
}

int SNEPPX_grad_opt_set_lr(SNEPPXGradientOptimizer* opt, double lr) {
    (void)opt; (void)lr;
    return 0;
}

double SNEPPX_grad_opt_get_lr(const SNEPPXGradientOptimizer* opt) {
    (void)opt;
    return 0.001;
}

int SNEPPX_grad_opt_add_param_group(SNEPPXGradientOptimizer* opt, SNEPPXTensor** params, size_t num_params, double lr, double weight_decay) {
    (void)opt; (void)params; (void)num_params; (void)lr; (void)weight_decay;
    return 0;
}

int SNEPPX_grad_opt_clip_grad_norm(SNEPPXTensor** grads, size_t num_grads, double max_norm) {
    (void)grads; (void)num_grads; (void)max_norm;
    return 0;
}

int SNEPPX_grad_opt_clip_grad_value(SNEPPXTensor** grads, size_t num_grads, double min_val, double max_val) {
    (void)grads; (void)num_grads; (void)min_val; (void)max_val;
    return 0;
}

SNEPPXLRScher* SNEPPX_lr_scheduler_create(SNEPPXLRScherType type, double initial_lr, size_t total_steps, const double* milestones, size_t num_milestones) {
    (void)type; (void)initial_lr; (void)total_steps; (void)milestones; (void)num_milestones;
    return (SNEPPXLRScher*)calloc(1, sizeof(SNEPPXLRScher));
}

void SNEPPX_lr_scheduler_destroy(SNEPPXLRScher* scheduler) {
    free(scheduler);
}

double SNEPPX_lr_scheduler_get_lr(const SNEPPXLRScher* scheduler, size_t step) {
    (void)scheduler; (void)step;
    return 0.001;
}

int SNEPPX_lr_scheduler_step(SNEPPXLRScher* scheduler) {
    (void)scheduler;
    return 0;
}
