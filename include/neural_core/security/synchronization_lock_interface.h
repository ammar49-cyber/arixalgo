#ifndef SNEPPX_LOCK_H
#define SNEPPX_LOCK_H

#include <stddef.h>

typedef struct SNEPPXLock SNEPPXLock;

SNEPPXLock* SNEPPX_lock_init(void);
void SNEPPX_lock_destroy(SNEPPXLock* lock);
void SNEPPX_lock_acquire(SNEPPXLock* lock);
void SNEPPX_lock_release(SNEPPXLock* lock);
int SNEPPX_lock_try_acquire(SNEPPXLock* lock);
int SNEPPX_lock_is_held(const SNEPPXLock* lock);

int SNEPPX_mlock(void* ptr, size_t len);
int SNEPPX_munlock(void* ptr, size_t len);
int SNEPPX_mlockall_possible(void);

#endif
