#include "generative_adversarial_network.h"
#include <stdlib.h>

void* SNEPPX_gan_create(size_t latent_dim, size_t hidden_dim, size_t output_dim, int use_batch_norm, int use_spectral_norm) {
    (void)latent_dim; (void)hidden_dim; (void)output_dim; (void)use_batch_norm; (void)use_spectral_norm;
    return NULL;
}

void SNEPPX_gan_destroy(void* gan) { free(gan); }

int SNEPPX_gan_generate(void* gan, const float* noise, size_t num_samples, float* output) {
    (void)gan; (void)noise; (void)num_samples; (void)output;
    return 0;
}

int SNEPPX_gan_train_step(void* gan, const float* real_samples, size_t num_samples, float* g_loss, float* d_loss) {
    (void)gan; (void)real_samples; (void)num_samples; (void)g_loss; (void)d_loss;
    return 0;
}
