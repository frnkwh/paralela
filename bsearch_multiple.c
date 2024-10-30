#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MAX_THREADS 64
#define SEARCH_ELEMENTS 100000
#define MAX_TOTAL_ELEMENTS (1000 * 1000 * 16)

typedef struct {
        long long *inputArray;
        long long *searchArray;
        long long *resultsArray;
        int nTotalElements;
        int nSearchElements;
        int startIndex;
        int endIndex;
} SearchTask;

pthread_t Thread[MAX_THREADS];

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

        for (int i = task->startIndex; i < task->endIndex; i++) {
                long long target = task->searchArray[i];
                long long left = 0, right = task->nTotalElements;

                while (left < right) {
                        int mid = left + (right - left) / 2;

                        if (task->inputArray[mid] < target) {
                                left = mid + 1;
                        } else {
                                right = mid;
                        }
                }

                task->resultsArray[i] = left; // Armazenar o resultado da busca
        }

        free(task);
        return NULL;
}

int main(int argc, char *argv[]) {
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

        int blockSize = (SEARCH_ELEMENTS + nThreads - 1) / nThreads; // Tamanho do bloco
        for (int i = 0; i < nThreads; i++) {
                SearchTask *task = malloc(sizeof(SearchTask));
                task->inputArray = inputArray;
                task->searchArray = searchArray;
                task->resultsArray = resultsArray;
                task->nTotalElements = nTotalElements;
                task->nSearchElements = SEARCH_ELEMENTS;
                task->startIndex = i * blockSize;
                task->endIndex = (i + 1) * blockSize < SEARCH_ELEMENTS ? (i + 1) * blockSize : SEARCH_ELEMENTS;

                pthread_create(&Thread[i], NULL, bsearch_lower_bound_task, task);
        }

        for (int i = 0; i < nThreads; i++) {
                pthread_join(Thread[i], NULL);
        }

        //for (int i = 0; i < SEARCH_ELEMENTS; i++) {
        //        printf("Insertion point for %lld is %lld\n", searchArray[i], resultsArray[i]);
        //}

        free(inputArray);
        free(searchArray);
        free(resultsArray);

        return 0;
}
