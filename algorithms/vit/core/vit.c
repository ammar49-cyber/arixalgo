#include "vision_transformer.h"
#include <stdlib.h>

void* SNEPPX_vit_create(size_t img_size, size_t patch_size, size_t in_channels, size_t num_classes, size_t hidden_dim, size_t num_heads, size_t num_layers, float dropout) {
    (void)img_size; (void)patch_size; (void)in_channels; (void)num_classes; (void)hidden_dim; (void)num_heads; (void)num_layers; (void)dropout;
    return NULL;
}

void SNEPPX_vit_destroy(void* vit) { free(vit); }

int SNEPPX_vit_forward(void* vit, const float* images, size_t batch_size, float* logits) {
    (void)vit; (void)images; (void)batch_size; (void)logits;
    return 0;
}

int SNEPPX_vit_extract_features(void* vit, const float* images, size_t batch_size, float* features) {
    (void)vit; (void)images; (void)batch_size; (void)features;
    return 0;
}
