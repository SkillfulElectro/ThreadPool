#include "ThreadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void example_task(void *arg) {
    void **args = (void**) arg;
    int length = *(int*)args[0];
    int *array = (int*)args[1];

    for (int i = 0; i < length; i++) {
        printf("Processing number: %d\n", array[i]);
        sleep(1);
    }
}

int main() {
    ThreadPool pool;
    if (initialize_thread_pool(&pool) != 0) {
        // 0 == success
        fprintf(stderr, "Failed to initialize thread pool\n");
        return -1;
    }

    int numbers[] = {1, 2, 3, 4, 5};
    int length = sizeof(numbers) / sizeof(numbers[0]);

    if (add_task_to_pool(&pool, example_task, 2, &length, numbers) == 0) {
        // 0 == fail == 0 number of available threads
        printf("No available threads for processing the array\n");
    }

    wait_for_all_threads(&pool); // Wait for the task to complete

    destroy_thread_pool(&pool);
    return 0;
}
