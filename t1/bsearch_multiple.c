#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "chrono.h"

#define MAX_THREADS 64
#define SEARCH_ELEMENTS 100000
#define MAX_TOTAL_ELEMENTS 16000000

typedef struct {
    long long *searchArray;
    long long *result;
    int index;
    int nThreads;
    long long *inputArray;
    int nTotalElements;
} SearchTask;

pthread_t Thread[MAX_THREADS];
int my_thread_id[MAX_THREADS];

long long operation_count = 0;
pthread_mutex_t count_mutex;
pthread_barrier_t myBarrier;


long long *createArray(int size) {
    long long *newArr = malloc(sizeof(long long) * size);
    if (!newArr) return NULL;
    return newArr;
}

int compare(const void *a, const void *b) {
    return (*(long long *)a - *(long long *)b);
}

void *bsearch_lower_bound_task(void *arg) {

    SearchTask *task = (SearchTask *)arg;

    long long local_operation_count = 0;

    for (int i = task->index; i < SEARCH_ELEMENTS; i += task->nThreads) {
        long long left = 0, right = task->nTotalElements;
        long long target = task->searchArray[i];
        while (left < right) {
            int mid = left + (right - left) / 2;

            local_operation_count++;
            if (task->inputArray[mid] < target) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }

        task->result[i] = left;
    }

    free(task);
    pthread_barrier_wait(&myBarrier);

    pthread_mutex_lock(&count_mutex);
    operation_count += local_operation_count;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
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

    long long *inputArray = createArray(nTotalElements);
    long long *searchArray = createArray(SEARCH_ELEMENTS);
    long long *resultsArray = createArray(SEARCH_ELEMENTS);

    for (int i = 0; i < nTotalElements; i++) {
        inputArray[i] = rand() % nTotalElements;
    }
    for (int i = 0; i < SEARCH_ELEMENTS; i++) {
        searchArray[i] = rand() % nTotalElements;
    }

    qsort(inputArray, nTotalElements, sizeof(long long), compare);


    // Inicializa a barreira para sincronizar as threads
    pthread_barrier_init(&myBarrier, NULL, nThreads);

    chrono_reset(&parallelReductionTime);
    chrono_start(&parallelReductionTime);

    for (int i = 0; i < nThreads; i++) {
        my_thread_id[i] = i;
        SearchTask *task = malloc(sizeof(SearchTask));
        task->searchArray = searchArray;
        task->result = resultsArray;
        task->index = i;
        task->nThreads = nThreads;
        task->inputArray = inputArray;
        task->nTotalElements = nTotalElements;
        pthread_create(&Thread[i], NULL, bsearch_lower_bound_task, task);

    }

    for (int i = 0; i < nThreads; i++) {
        pthread_join(Thread[i], NULL);
    }

    chrono_stop(&parallelReductionTime);

    double total_time_in_seconds = (double)chrono_gettotal(&parallelReductionTime) / 1e9;
    double ops_per_second = (double)operation_count / total_time_in_seconds;

    printf("%.6f\n", total_time_in_seconds);
    printf("%.2f\n", ops_per_second);


    pthread_barrier_destroy(&myBarrier);

    free(inputArray);
    free(searchArray);
    free(resultsArray);

    return 0;
}
