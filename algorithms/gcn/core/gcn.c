#include "graph_convolutional_network.h"
#include <stdlib.h>

void* SNEPPX_gcn_create(size_t in_features, size_t out_features, size_t hidden_features, int num_layers, float dropout) {
    (void)in_features; (void)out_features; (void)hidden_features; (void)num_layers; (void)dropout;
    return NULL;
}

void SNEPPX_gcn_destroy(void* gcn) { free(gcn); }

int SNEPPX_gcn_forward(void* gcn, const float* adj, const float* features, size_t num_nodes, float* output) {
    (void)gcn; (void)adj; (void)features; (void)num_nodes; (void)output;
    return 0;
}
