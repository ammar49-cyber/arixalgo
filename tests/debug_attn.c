#include "system_architecture_definitions.h"
#include "differentiable_training_pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(void) {
    arix_mem_pool_init();

    ArixArchConfig arch_cfg = arix_arch_config_default();
    arch_cfg.input_dim = 8; arch_cfg.output_dim = 8;
    arch_cfg.enable_hss = 0; arch_cfg.enable_ser = 0;
    arch_cfg.enable_attention = 1;
    arch_cfg.attention_config.d_model = 8;
    arch_cfg.attention_config.num_heads = 2;
    arch_cfg.attention_config.head_dim = 4;
    arch_cfg.attention_config.use_causal_mask = 0;
    arch_cfg.attention_config.use_rope = 0;
    arch_cfg.vocab_size = 16;

    printf("creating model...\n");
    ArixModel* model = arix_model_create(&arch_cfg);
    printf("model=%p attn=%p embed=%p\n", model, model->attention, model->embed_weight);
    if (!model) return 1;

    /* Test: arix_model_forward with 2D input */
    size_t B=1, S=4, VS=16;
    size_t in_shape[]={B,S};
    ArixTensor* input = arix_tensor_randn(in_shape, 2, ARIX_FLOAT32);
    float* id = (float*)input->data;
    for (size_t i=0; i<B*S; i++) id[i]=(int)(id[i]*2+5)%VS;

    ArixTensor* output = NULL;
    printf("calling arix_model_forward...\n");
    int ret = arix_model_forward(model, input, &output);
    printf("ret=%d output=%p\n", ret, output);
    if (output) {
        printf("output: ndim=%zu shape=[%zu,%zu,%zu] size=%zu\n",
               output->ndim,
               output->shape[0], output->ndim>=2?output->shape[1]:0, output->ndim>=3?output->shape[2]:0,
               output->size);
        printf("destroying output...\n");
        arix_tensor_destroy(output);
    }

    printf("destroying input...\n");
    arix_tensor_destroy(input);
    printf("destroying model...\n");
    arix_model_destroy(model);
    printf("DONE\n");
    return 0;
}
