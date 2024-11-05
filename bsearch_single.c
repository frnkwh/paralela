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
    int targetIndex;
    int thread_id;
} SearchTask;

pthread_t threads[MAX_THREADS];
pthread_mutex_t count_mutex;

long long operation_count = 0;

void *parallel_bsearch(void *arg) {
    SearchTask *task = (SearchTask *)arg;
    int nTotalElements = task->nTotalElements;
    int nThreads = task->nThreads;
    long long *inputArray = task->inputArray;
    long long target = task->searchArray[task->targetIndex];

    int segment_size = nTotalElements / nThreads;
    int local_left = task->thread_id * segment_size;
    int local_right = (task->thread_id == nThreads - 1) ? nTotalElements : local_left + segment_size;

    int local_operation_count = 0;
    int found_index = -1;

    while (local_left < local_right) {
        int mid = local_left + (local_right - local_left) / 2;
        local_operation_count++;

        if (inputArray[mid] == target) {
            found_index = mid;
            break;
        } else if (inputArray[mid] < target) {
            local_left = mid + 1;
        } else {
            local_right = mid;
        }
    }

    if (found_index != -1) {
        pthread_mutex_lock(&count_mutex);
        task->result[task->targetIndex] = found_index;
        pthread_mutex_unlock(&count_mutex);
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

    chrono_reset(&parallelReductionTime);
    chrono_start(&parallelReductionTime);

    for (int i = 0; i < SEARCH_ELEMENTS; i++) {
        SearchTask task;
        task.inputArray = inputArray;
        task.searchArray = searchArray;
        task.result = resultsArray;
        task.nTotalElements = nTotalElements;
        task.nThreads = nThreads;
        task.targetIndex = i;

        for (int j = 0; j < nThreads; j++) {
            task.thread_id = j;
            pthread_create(&threads[j], NULL, parallel_bsearch, &task);
        }

        for (int j = 0; j < nThreads; j++) {
            pthread_join(threads[j], NULL);
        }
    }

    chrono_stop(&parallelReductionTime);

    double total_time_in_seconds = (double)chrono_gettotal(&parallelReductionTime) / 1e9;
    double ops_per_second = (double)operation_count / total_time_in_seconds;

    printf("Tempo total: %.6f segundos\n", total_time_in_seconds);
    printf("Operações por segundo: %.2f\n", ops_per_second);

    pthread_mutex_destroy(&count_mutex);

    free(inputArray);
    free(searchArray);
    free(resultsArray);

    return 0;
}

