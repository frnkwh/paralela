#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "chrono.h"

#define MAX_THREADS 64

typedef struct {
        long long target;
        int *result;
        int index;
        long long *inputArray;
        int nElements;
} SearchTask;

pthread_t threads[MAX_THREADS];
long long operation_count = 0; // Contador global de operações
pthread_mutex_t count_mutex; // Mutex para proteger o contador

int compare(const void *a, const void *b) {
        return (*(long long *)a - *(long long *)b);
}

void *bsearch_lower_bound_task(void *arg) {
        SearchTask *task = (SearchTask *)arg;
        long long target = task->target;
        long long left = 0, right = task->nElements;

        while (left < right) {
                int mid = left + (right - left) / 2;

                // Incrementa o contador de operações
                pthread_mutex_lock(&count_mutex);
                operation_count++; // Contar a operação
                pthread_mutex_unlock(&count_mutex);

                if (task->inputArray[mid] < target) {
                        left = mid + 1;
                } else {
                        right = mid;
                }
        }

        task->result[task->index] = left; // Armazenar o resultado
        free(task); // Liberar memória da tarefa
        return NULL;
}

void generate_sorted_array(long long *array, int size) {
        for (int i = 0; i < size; i++) {
                array[i] = rand() % (size * 10); // Valores aleatórios
        }
        qsort(array, size, sizeof(long long), compare);
}

long long *allocArray(long long size) {
        return malloc(sizeof(long long) * size);
}

int main() {
        chronometer_t parallelReductionTime;
        pthread_mutex_init(&count_mutex, NULL); // Inicializa o mutex

        int nElements, nTargets;
        scanf("%d %d", &nElements, &nTargets);

        long long *inputArray = allocArray(nElements);
        long long targets[nTargets];
        int results[nTargets];

        srand(time(NULL));
        generate_sorted_array(inputArray, nElements);

        for (int i = 0; i < nTargets; i++) {
                scanf("%lld", &targets[i]);
        }

        chrono_reset(&parallelReductionTime);
        chrono_start(&parallelReductionTime);

        for (int i = 0; i < nTargets; i++) {
                SearchTask *task = malloc(sizeof(SearchTask));
                task->target = targets[i];
                task->result = results;
                task->index = i;
                task->inputArray = inputArray; // Passa o vetor de entrada
                task->nElements = nElements;     // Passa o número de elementos
                pthread_create(&threads[i], NULL, bsearch_lower_bound_task, task);
        }

        for (int i = 0; i < nTargets; i++) {
                pthread_join(threads[i], NULL);
        }

        chrono_stop(&parallelReductionTime);

        for (int i = 0; i < nTargets; i++) {
                printf("Insertion point for %lld is %d\n", targets[i], results[i]);
        }

        double total_time_in_seconds = (double)chrono_gettotal(&parallelReductionTime) / 1e9; // Converte ns para segundos
        double ops_per_second = (double)operation_count / total_time_in_seconds; // Calcula operações por segundo

        printf("Total time taken: %.6f seconds\n", total_time_in_seconds);
        printf("Total operations: %lld\n", operation_count);
        printf("Vazão (operações por segundo): %.2f OPS\n", ops_per_second);

        free(inputArray);
        pthread_mutex_destroy(&count_mutex); // Destrói o mutex

        return 0;
}

