#ifndef ARIX_THREAD_H
#define ARIX_THREAD_H

#include <stddef.h>

typedef struct ArixThreadPool ArixThreadPool;

ArixThreadPool* arix_threadpool_create(size_t num_threads);
void arix_threadpool_destroy(ArixThreadPool* pool);
void arix_threadpool_submit(ArixThreadPool* pool, void (*task)(void*), void* arg);
void arix_threadpool_wait(ArixThreadPool* pool);
size_t arix_threadpool_default_count(void);

#endif /* ARIX_THREAD_H */
