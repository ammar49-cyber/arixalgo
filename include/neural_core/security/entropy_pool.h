#ifndef ARIX_ENTROPY_POOL_H
#define ARIX_ENTROPY_POOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_ENTROPY_POOL_SIZE 256
#define ARIX_ENTROPY_THRESHOLD 128
#define ARIX_ENTROPY_SOURCES 8

typedef enum {
    ARIX_ENTROPY_SOURCE_RDTSC = 0,
    ARIX_ENTROPY_SOURCE_OS = 1,
    ARIX_ENTROPY_SOURCE_INTERRUPT_JITTER = 2,
    ARIX_ENTROPY_SOURCE_NETWORK = 3,
    ARIX_ENTROPY_SOURCE_DISK_TIMING = 4,
    ARIX_ENTROPY_SOURCE_MOUSE = 5,
    ARIX_ENTROPY_SOURCE_KEYBOARD = 6,
    ARIX_ENTROPY_SOURCE_MICROPHONE = 7,
} ArixEntropySource;

typedef struct {
    uint8_t pool[ARIX_ENTROPY_POOL_SIZE];
    int pool_index;
    int entropy_estimate;
    int source_available[ARIX_ENTROPY_SOURCES];
    uint64_t last_collection[ARIX_ENTROPY_SOURCES];
} ArixEntropyPool;

int  arix_entropy_pool_init(ArixEntropyPool* ep);
int  arix_entropy_pool_add(ArixEntropyPool* ep, ArixEntropySource src, const uint8_t* data, size_t len);
int  arix_entropy_pool_add_rdtsc(ArixEntropyPool* ep);
int  arix_entropy_pool_add_os(ArixEntropyPool* ep);
int  arix_entropy_pool_collect(ArixEntropyPool* ep);
int  arix_entropy_pool_get(ArixEntropyPool* ep, uint8_t* out, size_t out_len);
int  arix_entropy_pool_estimate(const ArixEntropyPool* ep);
void arix_entropy_pool_stir(ArixEntropyPool* ep);

#ifdef __cplusplus
}
#endif
#endif
