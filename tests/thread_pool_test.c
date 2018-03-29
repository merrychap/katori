#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "thread_pool.h"


#define THREAD        32
#define QUEUE         256
#define LITTLE_THREAD 4
#define BIG_QUEUE     8192


int tasks = 0, done = 0, left = 0;
pthread_mutex_t lock;

void dummy_task_slow(void *arg) {
    usleep(10000);
    pthread_mutex_lock(&lock);
    done++;
    pthread_mutex_unlock(&lock);
}


void dummy_task_quick(void *arg) {
    usleep(100);
    pthread_mutex_lock(&lock);
    left--;
    pthread_mutex_unlock(&lock);
}


int main(int argc, char **argv) {
    thread_pool_t *tpool;

    pthread_mutex_init(&lock, NULL);

    fprintf(stderr, "\n[*] TEST: general work\n");

    assert((tpool = thread_pool_init(THREAD, QUEUE)) != NULL);
    fprintf(stderr, "[*] Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);

    while (thread_pool_add(tpool, &dummy_task_slow, NULL) == 0) {
        pthread_mutex_lock(&lock);
        tasks++;
        pthread_mutex_unlock(&lock);
    }

    fprintf(stderr, "[*] Added %d tasks\n", tasks);

    while ((tasks / 2) > done) {
        usleep(10000);
    }

    assert(thread_pool_kill(tpool, urgent_shutdown) == 0);
    fprintf(stderr, "[*] Did %d tasks\n\n", done);
    
    fprintf(stderr, "===================\n[*] TEST: shutdown\n\n");
    fprintf(stderr, "[*] Urgent shutdown.\n");

    left = BIG_QUEUE;
    tpool = thread_pool_init(LITTLE_THREAD, BIG_QUEUE);
    for (size_t i = 0; i < BIG_QUEUE; i++)
        assert(thread_pool_add(tpool, &dummy_task_quick, NULL) == 0);
    assert(thread_pool_kill(tpool, urgent_shutdown) == 0);
    assert(left > 0);

    fprintf(stderr, "[+] Urgent shutdown completed.\n\n");

    fprintf(stderr, "[*] Complete shutdown.\n");
    left = BIG_QUEUE;
    tpool = thread_pool_init(LITTLE_THREAD, BIG_QUEUE);
    for (size_t i = 0; i < BIG_QUEUE; i++)
        assert(thread_pool_add(tpool, &dummy_task_quick, NULL) == 0);
    assert(thread_pool_kill(tpool, complete_shutdown) == 0);
    assert(left == 0);

    fprintf(stderr, "[+] Complete shutdown completed.\n");

    pthread_mutex_destroy(&lock);

    return 0;
}