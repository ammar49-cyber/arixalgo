#include "recurrent_neural_network.h"
#include <stdlib.h>

void* SNEPPX_rnn_create(size_t input_size, size_t hidden_size, size_t num_layers, int bidirectional, float dropout, const char* rnn_type) {
    (void)input_size; (void)hidden_size; (void)num_layers; (void)bidirectional; (void)dropout; (void)rnn_type;
    return NULL;
}

void SNEPPX_rnn_destroy(void* rnn) { free(rnn); }

int SNEPPX_rnn_forward(void* rnn, const float* input, size_t seq_len, size_t batch_size, float* output, float* hidden) {
    (void)rnn; (void)input; (void)seq_len; (void)batch_size; (void)output; (void)hidden;
    return 0;
}
