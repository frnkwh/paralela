#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "chrono.h"

#define MAX_THREADS 64
#define SEARCH_ELEMENTS 100000
#define MAX_TOTAL_ELEMENTS 16000000

typedef struct {
    long long *inputArray;
    long long *searchArray;
    long long *result;
    int nTotalElements;
    int nThreads;
    int targetIndex; // Index of the target in searchArray
    int currentStep; // Current binary search step
} SearchTask;

pthread_t threads[MAX_THREADS];
pthread_barrier_t myBarrier;
pthread_mutex_t count_mutex;

long long operation_count = 0;

void *parallel_bsearch(void *arg) {
    SearchTask *task = (SearchTask *)arg;
    int thread_id = task->currentStep;

    int nTotalElements = task->nTotalElements;
    int nThreads = task->nThreads;
    long long *inputArray = task->inputArray;
    long long target = task->searchArray[task->targetIndex];

    long long local_operation_count = 0;
    int segment_size = nTotalElements / nThreads;

    long long left = 0, right = nTotalElements;

    while (left < right) {
        
        // Define each thread's search segment
        int local_left = left + thread_id * segment_size;
        int local_right = local_left + segment_size;

        // Adjust the bounds to fit within the search range
        if (local_right > right) local_right = right;
        
        // Each thread checks the midpoint of its segment
        int mid = local_left + (local_right - local_left) / 2;

        local_operation_count++;
        
        // Check if the current segment's midpoint value is less than target
        if (inputArray[mid] < target) {
            local_left = mid + 1;
        } else {
            local_right = mid;
        }

        // Synchronize threads to share updated bounds
        pthread_barrier_wait(&myBarrier);
        
        // Determine global bounds based on the threads' findings
        if (thread_id == 0) {
            // Thread 0 calculates new global bounds by finding the minimum left and maximum right
            left = local_left;
            right = local_right;
            for (int i = 1; i < nThreads; i++) {
                left = (left > local_left) ? local_left : left;
                right = (right < local_right) ? local_right : right;
            }
        }

        pthread_barrier_wait(&myBarrier);
    }

    if (thread_id == 0) {
        task->result[task->targetIndex] = left; // Store the result only once
    }

    pthread_mutex_lock(&count_mutex);
    operation_count += local_operation_count;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
}

int compare(const void *a, const void *b) {
    return (*(long long *)a - *(long long *)b);
}

int main(int argc, char *argv[]) {
    chronometer_t parallelReductionTime;

    if (argc != 3) {
        printf("usage: %s <nTotalElements> <nThreads>\n", argv[0]);
        return 0;
    }

    int nTotalElements = atoi(argv[1]);
    int nThreads = atoi(argv[2]);
    if (nThreads > MAX_THREADS) {
        printf("<nThreads> must be less than %d\n", MAX_THREADS);
        return 0;
    }

    long long *inputArray = malloc(sizeof(long long) * nTotalElements);
    long long *searchArray = malloc(sizeof(long long) * SEARCH_ELEMENTS);
    long long *resultsArray = malloc(sizeof(long long) * SEARCH_ELEMENTS);

    for (int i = 0; i < nTotalElements; i++) {
        inputArray[i] = rand() % nTotalElements;
    }
    for (int i = 0; i < SEARCH_ELEMENTS; i++) {
        searchArray[i] = rand() % nTotalElements;
    }

    qsort(inputArray, nTotalElements, sizeof(long long), compare);

    pthread_mutex_init(&count_mutex, NULL);
    pthread_barrier_init(&myBarrier, NULL, nThreads);

    chrono_reset(&parallelReductionTime);
    chrono_start(&parallelReductionTime);

    for (int i = 0; i < SEARCH_ELEMENTS; i++) {
        SearchTask *task = malloc(sizeof(SearchTask));
        task->inputArray = inputArray;
        task->searchArray = searchArray;
        task->result = resultsArray;
        task->nTotalElements = nTotalElements;
        task->nThreads = nThreads;
        task->targetIndex = i;

        // Start nThreads to work together on each search element
        for (int j = 0; j < nThreads; j++) {
            task->currentStep = j;
            pthread_create(&threads[j], NULL, parallel_bsearch, task);
        }

        for (int j = 0; j < nThreads; j++) {
            pthread_join(threads[j], NULL);
        }

        free(task);
    }

    chrono_stop(&parallelReductionTime);

    double total_time_in_seconds = (double)chrono_gettotal(&parallelReductionTime) / 1e9;
    double ops_per_second = (double)operation_count / total_time_in_seconds;

    printf("%.6f\n", total_time_in_seconds);
    printf("%.2f\n", ops_per_second);

    pthread_barrier_destroy(&myBarrier);
    pthread_mutex_destroy(&count_mutex);

    free(inputArray);
    free(searchArray);
    free(resultsArray);

    return 0;
}

