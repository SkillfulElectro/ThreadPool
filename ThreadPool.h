#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

// Thread structure
typedef struct {
    pthread_t thread;
    int is_busy;
    void (*task)(void*);
    void *arg;
    pthread_mutex_t lock;
} Thread;

// Thread pool structure
typedef struct {
    Thread *threads;
    int max_threads;
    int active_threads;
    pthread_mutex_t pool_lock;
} ThreadPool;

// Task wrapper structure
typedef struct {
    void (*task)(void*);
    void *args;
} TaskWrapper;

// Function prototypes
int initialize_thread_pool(ThreadPool *pool);
int add_task_to_pool(ThreadPool *pool, void (*task)(void*), int num_args, ...);
void wait_for_all_threads(ThreadPool *pool);
void destroy_thread_pool(ThreadPool *pool);
void* pack_args(int num_args, va_list args);
void* thread_function(void* arg);

#endif // THREADPOOL_H
