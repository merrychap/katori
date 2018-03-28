#include <stdlib.h>


#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#define MAX_THREADS 64
#define QUEUE_SIZE  65536


typedef struct thread_pool_t thread_pool_t;

typedef enum {
    thread_pool_error        = -1,
    thread_pool_thread_error = -2,
    thread_pool_lock_error   = -3,
    thread_pool_queue_full   = -4,
    thread_pool_shutdown     = -5
} thread_pool_error_t;

typedef enum {
    direct_shutdown = 1
} thread_pool_shutdown_t;


thread_pool_t * thread_pool_init(size_t thread_count, size_t queue_size);

int thread_pool_add(thread_pool_t *tpool, void (* work)(void *), void *args);

int thread_pool_kill(thread_pool_t *tpool);

#endif