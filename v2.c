#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MAX_THREADS 64
#define SEARCH_ELEMENTS 100000
#define MAX_TOTAL_ELEMENTS (1000 * 1000 * 16)

typedef struct {
        long long target;
        long long *result;
        int index;
        long long *inputArray;
        int nTotalElements;
} SearchTask;

pthread_t Thread[MAX_THREADS];
long long operation_count = 0;
pthread_mutex_t count_mutex;
int nThreads;
int nTotalElements;

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
        long long target = task->target;
        long long left = 0, right = task->nTotalElements;

        while (left < right) {
                int mid = left + (right - left) / 2;
                pthread_mutex_lock(&count_mutex);
                operation_count++;
                pthread_mutex_unlock(&count_mutex);

                if (task->inputArray[mid] < target) {
                        left = mid + 1;
                } else {
                        right = mid;
                }
        }

        task->result[task->index] = left;
        free(task);
        return NULL;
}

int main(int argc, char *argv[]) {

        srand(time(NULL));

        if (argc != 3) {
                printf("usage: %s <nTotalElements> <nThreads>\n", argv[0]);
                return 0;
        }

        nTotalElements = atoi(argv[1]);
        nThreads = atoi(argv[2]);
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

        for (int i = 0; i < nTotalElements; i++) {
                printf("%lld ", inputArray[i]);
        }
        printf("\n");




        for (int i = 0; i < nThreads; i++) {
                SearchTask *task = malloc(sizeof(SearchTask));
                task->target = searchArray[i];
                task->result = resultsArray;
                task->index = i;
                task->inputArray = inputArray;
                task->nTotalElements = nTotalElements;

                if (pthread_create(&Thread[i], NULL, bsearch_lower_bound_task, task) != 0) {
                        perror("Failed to create thread");
                        free(task);
                        return 1;
                }
        }

        for (int i = 0; i < nThreads; i++) {
                pthread_join(Thread[i], NULL);
        }

        // Aqui você pode imprimir os resultados ou fazer o que precisar com `resultsArray`.
        for (int i = 0; i < nThreads; i++) {
                printf("Insertion point for %lld is %lld\n", searchArray[i], resultsArray[i]);
        }

        free(inputArray);
        free(searchArray);
        free(resultsArray);

        return 0;
}
