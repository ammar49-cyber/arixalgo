#include "transformer_model.h"
#include <stdlib.h>

void* SNEPPX_transformer_create(size_t vocab_size, size_t hidden_dim, size_t num_heads, size_t num_layers, size_t ffn_dim, float dropout, int use_rope) {
    (void)vocab_size; (void)hidden_dim; (void)num_heads; (void)num_layers; (void)ffn_dim; (void)dropout; (void)use_rope;
    return NULL;
}

void SNEPPX_transformer_destroy(void* model) { free(model); }

int SNEPPX_transformer_forward(void* model, const int* input_ids, size_t seq_len, float* logits) {
    (void)model; (void)input_ids; (void)seq_len; (void)logits;
    return 0;
}

int SNEPPX_transformer_generate(void* model, const int* prompt, size_t prompt_len, int* output, size_t max_len, int temperature, int top_k) {
    (void)model; (void)prompt; (void)prompt_len; (void)output; (void)max_len; (void)temperature; (void)top_k;
    return 0;
}
