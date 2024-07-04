# ThreadPool
Simple POSIX style thread pool lib

### Steps to use
```main.c
int initialize_thread_pool(ThreadPool *pool);

int add_task_to_pool(ThreadPool *pool, void (*task)(void*), int num_args, ...);

void wait_for_all_threads(ThreadPool *pool);

void destroy_thread_pool(ThreadPool *pool);
```
### task example
```main.c
void example_task(void *arg) {
    // Unpack the arguments
    void **args = (void**) arg;
    int length = *(int*)args[0];
    int *array = (int*)args[1];

    for (int i = 0; i < length; i++) {
        printf("Processing number: %d\n", array[i]);
        sleep(1); // Simulate some work
    }
}
```
- for complete example check out examples/main.c


## Enhancements for a Better ThreadPool
To improve the ThreadPool library, consider the following enhancements:

- Task Queue: Implement a task queue to manage tasks more efficiently. Tasks can be queued when all threads are busy, and idle threads can fetch tasks from the queue.Dynamic 
- Thread Management: Implement mechanisms to dynamically adjust the number of threads in the pool based on the workload. For example, increasing the number of threads when the task queue grows and reducing when it's low.
- Timeouts: Add support for task timeouts to avoid threads being blocked indefinitely by long-running tasks.
- Thread Reuse: Enhance the mechanism to reuse threads more effectively, avoiding thread creation and destruction overhead.
- Error Handling: Improve error handling and reporting mechanisms to provide more informative error messages and recover gracefully from failures.Thread Affinity: Implement thread affinity support to bind threads to specific CPU cores for better performance in some scenarios.
- Priority Scheduling: Introduce task priority levels to manage the execution order of tasks based on their importance.
- Scalability: Enhance the library to support distributed thread pools, allowing tasks to be processed across multiple machines.
