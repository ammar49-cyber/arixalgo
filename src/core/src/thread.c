#include "arix_thread.h"
#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
#endif

struct ArixThreadPool {
    size_t num_threads;
};

ArixThreadPool* arix_threadpool_create(size_t num_threads) {
    ArixThreadPool* pool = (ArixThreadPool*)malloc(sizeof(ArixThreadPool));
    if (!pool) return NULL;
    pool->num_threads = num_threads;
    return pool;
}

void arix_threadpool_destroy(ArixThreadPool* pool) {
    free(pool);
}

void arix_threadpool_submit(ArixThreadPool* pool, void (*task)(void*), void* arg) {
    (void)pool;
    if (task) task(arg);
}

void arix_threadpool_wait(ArixThreadPool* pool) {
    (void)pool;
}

size_t arix_threadpool_default_count(void) {
#ifdef __linux__
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n > 0) return (size_t)n;
#endif
    return 4;
}
