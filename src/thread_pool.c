#include <pthread.h>

#include "thread_pool.h"


typedef struct {
    void (*task)(void *);
    void *args;
} thread_pool_work_t;


typedef struct {
    thread_pool_work_t *works;
    size_t size;
    size_t wcount;
    size_t head;
    size_t tail;
} thread_pool_queue_t;


struct thread_pool_t {
    pthread_t *workers;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    thread_pool_queue_t *work_queue;
    
    size_t workers_count;
    size_t active;

    int shutdown;
};


int thread_pool_free(thread_pool_t *tpool) {
    
    if (tpool == NULL) return -1;
    
    free(tpool->workers);
    free(tpool->work_queue->works);
    free(tpool->work_queue);

    pthread_mutex_destroy(&(tpool->lock));
    pthread_cond_destroy(&(tpool->cond));

    free(tpool);

    return 0;
}


static int thread_pool_thread(void *_tpool) {
    thread_pool_t *tpool       = (thread_pool_t *) _tpool;
    thread_pool_queue_t *queue = tpool->work_queue;
    
    while (1) {
        if (pthread_mutex_lock(&(tpool->lock)) != 0) return thread_pool_lock_error;

        while ((tpool->work_queue->wcount == 0) && (!tpool->shutdown)) {
            pthread_cond_wait(&(tpool->cond), &(tpool->lock));
        }

        if (tpool->shutdown == direct_shutdown) break;

        thread_pool_work_t work;
        work = queue->works[queue->head];
        queue->head = (queue->head + 1) % queue->size;
        queue->wcount--;

        if (pthread_mutex_unlock(&(tpool->lock)) != 0) return thread_pool_lock_error;

        (*(work.task))(work.args);
    }

    tpool->active--;

    pthread_mutex_unlock(&(tpool->lock));
    pthread_exit(NULL);

    return 0;
}


thread_pool_t * thread_pool_init(size_t thread_count, size_t queue_size) {
    if (queue_size <= 0 || queue_size > QUEUE_SIZE || thread_count <= 0 || thread_count > MAX_THREADS) return NULL;

    thread_pool_t *tpool = (thread_pool_t *) malloc(sizeof(thread_pool_t));
    if (tpool == NULL) return NULL;

    tpool->workers_count    = 0;
    tpool->shutdown = 0;
    tpool->work_queue->head = tpool->work_queue->tail = 0;
    tpool->work_queue->size = queue_size;

    tpool->workers    = (pthread_t *) malloc(sizeof(pthread_t) * thread_count);
    tpool->work_queue = (thread_pool_queue_t *) malloc(sizeof(thread_pool_queue_t));
    tpool->work_queue->works = (thread_pool_work_t *) malloc(sizeof(thread_pool_work_t) * queue_size);

    if (pthread_mutex_init(&(tpool->lock), NULL) != 0 || 
        pthread_cond_init(&(tpool->cond), NULL) != 0 ||
        tpool->workers == NULL || tpool->work_queue == NULL || tpool->work_queue->works == NULL) {
        thread_pool_free(tpool);
    }

    for (size_t i = 0; i < thread_count; i++) {
        if (pthread_create(&(tpool->workers[i]), NULL, thread_pool_thread, (void *)tpool) != 0) {
            thread_pool_kill(tpool);
            return NULL;
        }
        tpool->workers_count++;
    }

    return tpool;
}


int thread_pool_add(thread_pool_t *tpool, void (* work)(void *), void *args) {
    if (tpool == NULL || work == NULL) return thread_pool_error;
    if (pthread_mutex_lock(&(tpool->lock)) != 0) return thread_pool_lock_error;

    thread_pool_queue_t *queue = tpool->work_queue;

    size_t next = (queue->tail + 1) % queue->size;

    if (queue->size == queue->wcount) return thread_pool_queue_full;
    if (tpool->shutdown)              return thread_pool_shutdown;

    queue->works[queue->tail].task = work;
    queue->works[queue->tail].args = args;
    queue->tail = next;
    queue->wcount++;

    if (pthread_cond_signal( &(tpool->cond)) != 0) return thread_pool_lock_error;
    if (pthread_mutex_unlock(&(tpool->lock)) != 0) return thread_pool_lock_error;

    return 0;
}
