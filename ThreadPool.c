#include "ThreadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

void* thread_function(void* arg) {
    Thread *thread = (Thread*) arg;
    while (1) {
        pthread_mutex_lock(&thread->lock);
        if (thread->is_busy) {
            TaskWrapper *wrapper = (TaskWrapper*)thread->arg;
            wrapper->task(wrapper->args);
            free(wrapper->args);
            free(wrapper);
            thread->is_busy = 0;
        }
        pthread_mutex_unlock(&thread->lock);
        usleep(100);  // Small sleep to prevent busy waiting
    }
    return NULL;
}

int initialize_thread_pool(ThreadPool *pool) {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    pool->max_threads = num_cores;
    pool->active_threads = 0;
    pool->threads = (Thread*) malloc(sizeof(Thread) * num_cores);
    pthread_mutex_init(&pool->pool_lock, NULL);

    if (pool->threads == NULL) {
        perror("Failed to allocate memory for threads");
        return -1;
    }

    for (int i = 0; i < num_cores; i++) {
        pool->threads[i].is_busy = 0;
        pool->threads[i].task = NULL;
        pool->threads[i].arg = NULL;
        pthread_mutex_init(&pool->threads[i].lock, NULL);
        if (pthread_create(&pool->threads[i].thread, NULL, thread_function, &pool->threads[i]) != 0) {
            perror("Failed to create thread");
            free(pool->threads);
            return -1;
        }
    }
    return 0;
}

void* pack_args(int num_args, va_list args) {
    size_t total_size = 0;
    va_list args_copy;
    va_copy(args_copy, args);
    
    for (int i = 0; i < num_args; i++) {
        void *arg = va_arg(args_copy, void*);
        size_t arg_size = sizeof(arg);
        total_size += arg_size;
    }
    
    va_end(args_copy);

    void *packed_args = malloc(total_size);
    if (packed_args == NULL) {
        perror("Failed to allocate memory for arguments");
        return NULL;
    }

    char *ptr = (char*)packed_args;
    for (int i = 0; i < num_args; i++) {
        void *arg = va_arg(args, void*);
        memcpy(ptr, &arg, sizeof(arg));
        ptr += sizeof(arg);
    }

    return packed_args;
}

int add_task_to_pool(ThreadPool *pool, void (*task)(void*), int num_args, ...) {
    va_list args;
    va_start(args, num_args);
    
    void *packed_args = pack_args(num_args, args);
    va_end(args);

    if (packed_args == NULL) {
        return 0;
    }

    for (int i = 0; i < pool->max_threads; i++) {
        pthread_mutex_lock(&pool->threads[i].lock);
        if (!pool->threads[i].is_busy) {
            pool->threads[i].is_busy = 1;
            TaskWrapper *wrapper = (TaskWrapper*) malloc(sizeof(TaskWrapper));
            wrapper->task = task;
            wrapper->args = packed_args;
            pool->threads[i].arg = wrapper;
            pthread_mutex_unlock(&pool->threads[i].lock);
            return 1;
        }
        pthread_mutex_unlock(&pool->threads[i].lock);
    }

    free(packed_args);
    return 0;
}

void wait_for_all_threads(ThreadPool *pool) {
    int all_idle;
    do {
        all_idle = 1;
        for (int i = 0; i < pool->max_threads; i++) {
            pthread_mutex_lock(&pool->threads[i].lock);
            if (pool->threads[i].is_busy) {
                all_idle = 0;
            }
            pthread_mutex_unlock(&pool->threads[i].lock);
        }
        usleep(100);  // Small sleep to prevent busy waiting
    } while (!all_idle);
}

void destroy_thread_pool(ThreadPool *pool) {
    for (int i = 0; i < pool->max_threads; i++) {
        pthread_cancel(pool->threads[i].thread);
        pthread_join(pool->threads[i].thread, NULL);
        pthread_mutex_destroy(&pool->threads[i].lock);
    }
    free(pool->threads);
    pthread_mutex_destroy(&pool->pool_lock);
}
