// Sparse Expert Routing (SER) Algorithm Implementation
// Part of SNEPPX-ALG v0.9.5.748

#include "neural_core/algorithms/ser.h"
#include <math.h>
#include <float.h>

// Softmax with load balancing loss computation
static float compute_routing_weights_and_loss(const float* input, float* output,
                                            float* routing_weights, size_t input_dim,
                                            int num_experts, int top_k) {
    // Compute router logits: logits = W_r * input + b_r
    float logits[num_experts];
    for (int e = 0; e < num_experts; e++) {
        logits[e] = 0.0f;
        for (int i = 0; i < input_dim; i++) {
            logits[e] += model->W_r[e][i] * input[i];
        }
        logits[e] += model->b_r[e];
    }
    
    // Compute softmax
    float softmax_sum = 0.0f;
    for (int e = 0; e < num_experts; e++) {
        logits[e] = expf(logits[e]);
        softmax_sum += logits[e];
    }
    
    for (int e = 0; e < num_experts; e++) {
        logits[e] /= softmax_sum;
    }
    
    // Top-k selection
    // Create array of expert indices
    int expert_indices[num_experts];
    for (int e = 0; e < num_experts; e++) {
        expert_indices[e] = e;
    }
    
    // Sort by probability
    for (int i = 0; i < num_experts - 1; i++) {
        for (int j = 0; j < num_experts - i - 1; j++) {
            if (logits[expert_indices[j]] < logits[expert_indices[j+1]]) {
                int temp = expert_indices[j];
                expert_indices[j] = expert_indices[j+1];
                expert_indices[j+1] = temp;
            }
        }
    }
    
    // Copy top-k
    float top_k_probs[top_k];
    int top_k_experts[top_k];
    float load_balance_loss = 0.0f;
    
    for (int k = 0; k < top_k; k++) {
        int expert_idx = expert_indices[num_experts - 1 - k];
        top_k_experts[k] = expert_idx;
        top_k_probs[k] = logits[expert_idx];
        
        // Expert assignment fraction
        float f_i = 1.0f / num_experts;  // Simplified
        float P_i = logits[expert_idx] / (top_k + 1);
        load_balance_loss += f_i * P_i;
    }
    
    load_balance_loss *= model->balance_alpha;
    
    // Copy to output
    for (int k = 0; k < top_k; k++) {
        output[k] = top_k_probs[k];
        routing_weights[k] = top_k_experts[k];
    }
    
    return load_balance_loss;
}

int SNEPPXSERModel_create(SNEPPXSERConfig* config, unsigned int seed) {
    if (!config) {
        return SNEPPX_ERROR_INVALID_PARAM;
    }
    
    // Set defaults
    if (config->d_expert == 0) {
        config->d_expert = 4 * config->d_model;
    }
    if (config->num_experts == 0) {
        config->num_experts = 8;
    }
    if (config->top_k == 0) {
        config->top_k = 2;
    }
    if (config->balance_alpha == 0) {
        config->balance_alpha = 0.01f;
    }
    
    // Allocate model
    SNEPPXSERModel* model = (SNEPPXSERModel*)malloc(sizeof(SNEPPXSERModel));
    if (!model) {
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    model->config = *config;
    model->seed = seed;
    
    // Allocate expert weights
    srand(seed);
    model->experts = (SNEPPXExpert**)malloc(config->num_experts * sizeof(SNEPPXExpert*));
    if (!model->experts) {
        free(model);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    for (int e = 0; e < config->num_experts; e++) {
        model->experts[e] = (SNEPPXExpert*)malloc(sizeof(SNEPPXExpert));
        if (!model->experts[e]) {
            // Cleanup
            for (int i = 0; i < e; i++) {
                free(model->experts[i]);
            }
            free(model->experts);
            free(model);
            return SNEPPX_ERROR_MEMORY_ALLOC;
        }
        
        model->experts[e]->weight = (float*)malloc(config->d_model * config->d_expert * sizeof(float));
        model->experts[e]->bias = (float*)malloc(config->d_expert * sizeof(float));
        model->experts[e]->activation = config->activation;
        
        if (!model->experts[e]->weight || !model->experts[e]->bias) {
            // Cleanup
            for (int i = 0; i <= e; i++) {
                if (model->experts[i]->weight) free(model->experts[i]->weight);
                if (model->experts[i]->bias) free(model->experts[i]->bias);
                free(model->experts[i]);
            }
            free(model->experts);
            free(model);
            return SNEPPX_ERROR_MEMORY_ALLOC;
        }
        
        // Initialize expert weights
        for (int i = 0; i < config->d_model * config->d_expert; i++) {
            model->experts[e]->weight[i] = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / config->d_model;
        }
        for (int i = 0; i < config->d_expert; i++) {
            model->experts[e]->bias[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
        }
    }
    
    // Router weights
    model->W_r = (float**)malloc(config->num_experts * sizeof(float*));
    model->b_r = (float*)malloc(config->num_experts * sizeof(float));
    if (!model->W_r || !model->b_r) {
        for (int e = 0; e < config->num_experts; e++) {
            if (model->experts[e]->weight) free(model->experts[e]->weight);
            if (model->experts[e]->bias) free(model->experts[e]->bias);
            free(model->experts[e]);
        }
        if (model->experts) free(model->experts);
        if (model->W_r) free(model->W_r);
        if (model->b_r) free(model->b_r);
        free(model);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    for (int e = 0; e < config->num_experts; e++) {
        model->W_r[e] = (float*)malloc(config->d_model * sizeof(float));
        if (!model->W_r[e]) {
            // Cleanup
            for (int i = 0; i < e; i++) {
                free(model->W_r[i]);
            }
            free(model->W_r);
            free(model->b_r);
            for (int i = 0; i < config->num_experts; i++) {
                if (model->experts[i]->weight) free(model->experts[i]->weight);
                if (model->experts[i]->bias) free(model->experts[i]->bias);
                free(model->experts[i]);
            }
            free(model->experts);
            free(model);
            return SNEPPX_ERROR_MEMORY_ALLOC;
        }
        
        for (int i = 0; i < config->d_model; i++) {
            model->W_r[e][i] = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / config->d_model;
        }
        model->b_r[e] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
    }
    
    return (int)model;
}

void SNEPPXSERModel_destroy(SNEPPXSERModel* model) {
    if (!model) {
        return;
    }
    
    for (int e = 0; e < model->config.num_experts; e++) {
        if (model->experts[e]->weight) {
            free(model->experts[e]->weight);
        }
        if (model->experts[e]->bias) {
            free(model->experts[e]->bias);
        }
        free(model->experts[e]);
    }
    
    if (model->experts) {
        free(model->experts);
    }
    
    for (int e = 0; e < model->config.num_experts; e++) {
        if (model->W_r[e]) {
            free(model->W_r[e]);
        }
    }
    if (model->W_r) {
        free(model->W_r);
    }
    if (model->b_r) {
        free(model->b_r);
    }
    
    free(model);
}

int SNEPPXSERModel_forward(SNEPPXSERModel* model, const SNEPPXTensor* input,
                          SNEPPXTensor** output, float* load_balance_loss) {
    if (!model || !input || !output || !load_balance_loss) {
        return SNEPPX_ERROR_INVALID_PARAM;
    }
    
    size_t input_elements = SNEPPX_tensor_num_elements(input);
    size_t batch_size = input_elements / model->config.d_model;
    
    // Allocate output
    size_t output_shape[] = {batch_size, model->config.d_model};
    *output = SNEPPX_tensor_create(output_shape, 2, SNEPPX_FLOAT32);
    if (!*output) {
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    float* output_data = (float*)malloc(batch_size * model->config.d_model * sizeof(float));
    if (!output_data) {
        SNEPPX_tensor_destroy(*output);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    SNEPPXTensor* output_tensor = SNEPPX_tensor_from_buffer(
        output_data, output_shape, 2, SNEPPX_FLOAT32);
    
    // Copy input for routing
    float* input_data = (float*)malloc(batch_size * model->config.d_model * sizeof(float));
    if (!input_data) {
        SNEPPX_tensor_destroy(*output);
        free(output_data);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    SNEPPX_tensor_to_host(input, input_data);
    
    // Process each sequence (simplified for batch processing)
    for (size_t b = 0; b < batch_size; b++) {
        float* x = input_data + b * model->config.d_model;
        float* y = output_data + b * model->config.d_model;
        memset(y, 0, model->config.d_model * sizeof(float));
        
        // Compute routing
        float* routing_weights = (float*)malloc(model->config.top_k * sizeof(float));
        int* routing_experts = (int*)malloc(model->config.top_k * sizeof(int));
        float loss = compute_routing_weights_and_loss(x, routing_weights,
                                                      routing_experts, model->config.d_model,
                                                      model->config.num_experts, model->config.top_k);
        
        *load_balance_loss += loss;
        
        // Dispatch input to selected experts
        float* expert_outputs[model->config.top_k];
        for (int k = 0; k < model->config.top_k; k++) {
            expert_outputs[k] = (float*)calloc(model->config.d_expert, sizeof(float));
            if (!expert_outputs[k]) {
                for (int i = 0; i < k; i++) {
                    free(expert_outputs[i]);
                }
                free(routing_weights);
                free(routing_experts);
                free(output_data);
                free(input_data);
                SNEPPX_tensor_destroy(*output);
                return SNEPPX_ERROR_MEMORY_ALLOC;
            }
            memset(expert_outputs[k], 0, model->config.d_expert * sizeof(float));
        }
        
        // Simulate expert dispatch (in real implementation, this would be parallel)
        for (int k = 0; k < model->config.top_k; k++) {
            int expert_idx = routing_experts[k];
            SNEPPXExpert* expert = model->experts[expert_idx];
            
            // Apply expert transformation: output = x * W + b
            for (int i = 0; i < model->config.d_expert; i++) {
                expert_outputs[k][i] = 0.0f;
                for (int j = 0; j < model->config.d_model; j++) {
                    int weight_idx = i * model->config.d_model + j;
                    expert_outputs[k][i] += x[j] * expert->weight[weight_idx];
                }
                expert_outputs[k][i] += expert->bias[i];
            }
        }
        
        // Combine expert outputs with routing weights
        for (int i = 0; i < model->config.d_model; i++) {
            y[i] = 0.0f;
            for (int k = 0; k < model->config.top_k; k++) {
                // Fuse method: weighted sum of expert outputs
                // (simplified implementation)
                y[i] += routing_weights[k] * model->current_fused[k][i];
            }
        }
        
        free(routing_weights);
        free(routing_experts);
        for (int k = 0; k < model->config.top_k; k++) {
            free(expert_outputs[k]);
        }
    }
    
    free(output_data);
    free(input_data);
    return SNEPPX_SUCCESS;
}

void SNEPPXSERModel_compute_load_balance_loss(SNEPPXSERModel* model, float* loss) {
    if (!model || !loss) {
        return;
    }
    
    *loss = 0.0f;
    int num_experts = model->config.num_experts;
    
    // In a real implementation, this would compute the actual load balance
    // based on recent routing statistics
    *loss = model->config.balance_alpha * num_experts * 0.01f;
}