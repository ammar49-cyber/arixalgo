#include "diffusion_model.h"
#include <stdlib.h>

void* SNEPPX_diffusion_create(size_t img_channels, size_t img_size, size_t hidden_dim, int num_timesteps, int noise_schedule_type) {
    (void)img_channels; (void)img_size; (void)hidden_dim; (void)num_timesteps; (void)noise_schedule_type;
    return NULL;
}

void SNEPPX_diffusion_destroy(void* model) { free(model); }

int SNEPPX_diffusion_sample(void* model, float* output, size_t num_samples, const float* cond) {
    (void)model; (void)output; (void)num_samples; (void)cond;
    return 0;
}

int SNEPPX_diffusion_train_step(void* model, const float* images, size_t num_samples, float* loss) {
    (void)model; (void)images; (void)num_samples; (void)loss;
    return 0;
}
