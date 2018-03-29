#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "thread_pool.h"

#define THREAD 4
#define SIZE   8192


int left;
pthread_mutex_t lock;

int error;


void dummy_task(void *arg) {
    usleep(100);
    pthread_mutex_lock(&lock);
    left--;
    pthread_mutex_unlock(&lock);

}

int main(int argc, char **argv) {
    thread_pool_t *pool;

    pthread_mutex_init(&lock, NULL);

    left = SIZE;
    pool = thread_pool_init(THREAD, SIZE);
    for(size_t i = 0; i < SIZE; i++) {
        assert(thread_pool_add(pool, &dummy_task, NULL) == 0);
    }
    assert(thread_pool_kill(pool, urgent_shutdown) == 0);
    assert(left > 0);

    left = SIZE;
    pool = thread_pool_init(THREAD, SIZE);
    for (size_t i = 0; i < SIZE; i++) {
        assert(thread_pool_add(pool, &dummy_task, NULL) == 0);
    }
    assert(thread_pool_kill(pool, complete_shutdown) == 0);
    assert(left == 0);

    pthread_mutex_destroy(&lock);

    return 0;
}