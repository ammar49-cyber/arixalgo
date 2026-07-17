// Hierarchical State Space (HSS) Algorithm Implementation
// Part of SNEPPX-ALG v0.9.5.748

#include "neural_core/algorithms/hss.h"
#include <math.h>
#include <float.h>

// Forward computation for HSS (single timestep)
static inline void hss_forward_timestep(const SNEPPXHSSConfig* config,
                                        float dt,
                                        const float* x_prev,
                                        float* h_prev,
                                        float* x_curr,
                                        float* h_curr,
                                        float* y_out) {
    
    // Numerical integration: h_k = h_{k-1} + dt * f(h_{k-1}, x_{k-1})
    // For linearization: f(h, x) = A*h + B*x
    // Thus: h_k = (I + dt*A)*h_{k-1} + dt*B*x_{k-1}
    
    // Precompute transition matrices
    float I_plus_dtA[config->d_state][config->d_state];
    float dt_B[config->d_state][config->d_model];
    
    // Initialize identity + dt*A
    for (int i = 0; i < config->d_state; i++) {
        for (int j = 0; j < config->d_state; j++) {
            I_plus_dtA[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // Add dt * A
    for (int i = 0; i < config->d_state; i++) {
        for (int j = 0; j < config->d_state; j++) {
            I_plus_dtA[i][j] += dt * config->A[i][j];
        }
    }
    
    // Compute dt * B
    for (int i = 0; i < config->d_state; i++) {
        for (int j = 0; j < config->d_model; j++) {
            dt_B[i][j] = dt * config->B[i][j];
        }
    }
    
    // Multiply: h_k = (I + dt*A) * h_{k-1} + dt*B * x_{k-1}
    float temp_state[config->d_state];
    float temp_input[config->d_state];
    
    // Matrix-vector multiplication: temp_state = (I + dt*A) * h_prev
    for (int i = 0; i < config->d_state; i++) {
        temp_state[i] = 0.0f;
        for (int j = 0; j < config->d_state; j++) {
            temp_state[i] += I_plus_dtA[i][j] * h_prev[j];
        }
    }
    
    // Matrix-vector multiplication: temp_input = dt*B * x_curr
    for (int i = 0; i < config->d_state; i++) {
        temp_input[i] = 0.0f;
        for (int j = 0; j < config->d_model; j++) {
            temp_input[i] += dt_B[i][j] * x_curr[j];
        }
    }
    
    // Combine results: h_k = temp_state + temp_input
    for (int i = 0; i < config->d_state; i++) {
        h_curr[i] = temp_state[i] + temp_input[i];
    }
    
    // Output: y_k = C * h_k
    for (int i = 0; i < config->d_model; i++) {
        y_out[i] = 0.0f;
        for (int j = 0; j < config->d_state; j++) {
            y_out[i] += config->C[i][j] * h_curr[j];
        }
        // Add residual connection
        y_out[i] += config->W[i][i] * x_curr[i];
    }
}

// Combined forward pass for sequence (we already have previous state)
int SNEPPX_hss_forward(SNEPPXHSSModel* model, const SNEPPXTensor* input,
                      SNEPPXTensor** output) {
    if (!model || !input || !output) {
        return SNEPPX_ERROR_INVALID_PARAM;
    }
    
    SNEPPXHSSConfig* config = &model->config;
    size_t input_elements = SNEPPX_tensor_num_elements(input);
    size_t seq_len = input_elements / config->d_model;
    
    // Allocate output tensor
    size_t output_shape[] = {seq_len, config->d_model};
    *output = SNEPPX_tensor_create(output_shape, 2, SNEPPX_FLOAT32);
    if (!*output) {
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    // Get input data
    float* input_data = (float*)malloc(input_elements * sizeof(float));
    if (!input_data) {
        SNEPPX_tensor_destroy(*output);
        *output = NULL;
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    SNEPPX_tensor_to_host(input, input_data);
    
    // Process sequence timestep by timestep
    for (size_t t = 0; t < seq_len; t++) {
        float* prev_x = (t > 0) ? 
            (float*)SNEPPX_tensor_to_host(
                SNEPPX_tensor_slice(*output, 1, t-1, 1, 1), &tmp_slice) : // Simplified
            NULL;
        
        float* prev_h = (t > 0) ? model->prev_h[t-1] : model->h0;
        float* curr_y = (float*)SNEPPX_tensor_to_host(
            SNEPPX_tensor_slice(*output, 1, t, 1, 1), &tmp_slice);
        
        // For now, use current timestep input as both x_prev and x_curr
        // Full implementation needs proper temporal flow
        float* x_curr = input_data + t * config->d_model;
        
        hss_forward_timestep(config, config->dt,
                           prev_h, prev_x,
                           x_curr, model->current_h,
                           curr_y);
        
        // Update previous state for next timestep
        if (t < seq_len - 1) {
            memcpy(model->prev_h[t], model->current_h, config->d_state * sizeof(float));
        }
    }
    
    free(input_data);
    return SNEPPX_SUCCESS;
}

// Initialize HSS model with default parameters
int SNEPPXHSSModel_create(SNEPPXHSSConfig* config, unsigned int seed) {
    if (!config) {
        return SNEPPX_ERROR_INVALID_PARAM;
    }
    
    // Set default values
    if (config->d_state == 0) {
        config->d_state = config->d_model / 4;
    }
    if (config->num_layers == 0) {
        config->num_layers = 1;
    }
    if (config->dt_min == 0) {
        config->dt_min = 0.001f;
    }
    if (config->dt_max == 0) {
        config->dt_max = 0.1f;
    }
    
    // Allocate model structure
    SNEPPXHSSModel* model = (SNEPPXHSSModel*)malloc(sizeof(SNEPPXHSSModel));
    if (!model) {
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    model->config = *config;
    model->seed = seed;
    
    // Initialize state matrices (simplified - in real implementation would use proper initialization)
    srand(seed);
    for (int i = 0; i < model->config.d_state; i++) {
        for (int j = 0; j < model->config.d_state; j++) {
            model->config.A[i][j] = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / model->config.d_state;
        }
        for (int j = 0; j < model->config.d_model; j++) {
            model->config.B[i][j] = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / model->config.d_model;
            model->config.C[j][i] = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / model->config.d_model;
            model->config.W[j][j] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f + 1.0f;
        }
    }
    
    // Initial state (zeros)
    model->h0 = (float*)calloc(model->config.d_state, sizeof(float));
    if (!model->h0) {
        free(model);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    // Previous states array
    size_t max_seq_len = 1024; // Maximum sequence length for testing
    model->prev_h = (float**)malloc(max_seq_len * sizeof(float*));
    if (!model->prev_h) {
        free(model->h0);
        free(model);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    for (size_t i = 0; i < max_seq_len; i++) {
        model->prev_h[i] = (float*)calloc(model->config.d_state, sizeof(float));
        if (!model->prev_h[i]) {
            // Cleanup already allocated
            for (size_t j = 0; j < i; j++) {
                free(model->prev_h[j]);
            }
            free(model->prev_h);
            free(model->h0);
            free(model);
            return SNEPPX_ERROR_MEMORY_ALLOC;
        }
    }
    
    model->current_h = (float*)calloc(model->config.d_state, sizeof(float));
    if (!model->current_h) {
        for (size_t i = 0; i < max_seq_len; i++) {
            free(model->prev_h[i]);
        }
        free(model->prev_h);
        free(model->h0);
        free(model);
        return SNEPPX_ERROR_MEMORY_ALLOC;
    }
    
    return (int)model;
}

void SNEPPXHSSModel_destroy(SNEPPXHSSModel* model) {
    if (!model) {
        return;
    }
    
    if (model->h0) {
        free(model->h0);
    }
    if (model->prev_h) {
        for (size_t i = 0; i < 1024; i++) {
            free(model->prev_h[i]);
        }
        free(model->prev_h);
    }
    if (model->current_h) {
        free(model->current_h);
    }
    
    free(model);
}