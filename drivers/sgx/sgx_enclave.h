#ifndef SNEPPX_SGX_ENCLAVE_H
#define SNEPPX_SGX_ENCLAVE_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int  SNEPPX_sgx_init(const char* enclave_path);
void SNEPPX_sgx_destroy(void);
int  SNEPPX_sgx_create_enclave(const char* name, size_t heap_size, size_t stack_size);
int  SNEPPX_sgx_destroy_enclave(void);
int  SNEPPX_sgx_call(const char* func_name, void* input, size_t input_len, void* output, size_t output_len);
int  SNEPPX_sgx_seal_data(const unsigned char* data, size_t data_len, unsigned char* sealed, size_t* sealed_len);
int  SNEPPX_sgx_unseal_data(const unsigned char* sealed, size_t sealed_len, unsigned char* data, size_t* data_len);
#ifdef __cplusplus
}
#endif
#endif
