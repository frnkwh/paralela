#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

#include "chrono.h"

#define DEBUG 0
//#define DEBUG 1

#define MAX_THREADS 64
#define LOOP_COUNT 1

#define SEARCH_ELEMENTS 100000

#define MAX_TOTAL_ELEMENTS (1000*1000*16)

typedef struct {
        long long target;
        long long *result;
        int index;
        long long *inputArray;
        int nTotalElements;
} SearchTask;


pthread_t Thread[MAX_THREADS];
int my_thread_id[MAX_THREADS];
long long operation_count = 0; // Contador global de operações
pthread_mutex_t count_mutex; // Mutex para proteger o contador

int nThreads;  // numero efetivo de threads
               // obtido da linha de comando  
int nTotalElements;  // numero total de elementos
               // obtido da linha de comando      
               
pthread_barrier_t myBarrier;

long long *createArray(int size) {
        long long *newArr = malloc(sizeof(long long) * size);
        if (!newArr)
                return NULL;

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
        
        pthread_barrier_wait(&myBarrier);

        return NULL;
}

int main(int argc, char *argv[]) {
        int i;
        chronometer_t parallelReductionTime;
        long long *searchArray;
        long long *inputArray;
        long long *resultsArray;


        if(argc != 3) {
                printf("usage: %s <nTotalElements> <nThreads>\n", argv[0]); 
                return 0;
        } else {
                nThreads = atoi(argv[2]);
                if (nThreads == 0) {
                        printf("usage: %s <nTotalElements> <nThreads>\n", argv[0]);
                        printf("<nThreads> can't be 0\n");
                        return 0;
                }     
                if (nThreads > MAX_THREADS) {  
                        printf("usage: %s <nTotalElements> <nThreads>\n" ,
                               argv[0]);
                        printf("<nThreads> must be less than %d\n", MAX_THREADS);
                        return 0;
                }     
                nTotalElements = atoi(argv[1]); 
                if (nTotalElements > MAX_TOTAL_ELEMENTS) {  
                        printf("usage: %s <nTotalElements> <nThreads>\n", argv[0]);
                        printf("<nTotalElements> must be up to %d\n", MAX_TOTAL_ELEMENTS);
                        return 0;
                }     
        }

        inputArray = createArray(nTotalElements);
        searchArray = createArray(SEARCH_ELEMENTS);
        resultsArray = createArray(SEARCH_ELEMENTS);

        printf("will use %d threads to search %d total elements\n\n", nThreads, nTotalElements);

        // Initialize inputArray
        for (int i = 0; i < nTotalElements; i++) {
                inputArray[i] = rand() % nTotalElements;
        }
        // Initialize searchArray
        for (int i = 0; i < SEARCH_ELEMENTS; i++) {
                searchArray[i] = rand() % nTotalElements;
        }

        // Sort input array
        qsort(inputArray, nTotalElements, sizeof(long long), compare);

        chrono_reset(&parallelReductionTime);

        pthread_barrier_init(&myBarrier, NULL, nThreads);

        // thread 0 será main
        my_thread_id[0] = 0;
        for (i = 1; i < nThreads; i++) {
                my_thread_id[i] = i;
                SearchTask *task = malloc(sizeof(SearchTask));
                task->target = searchArray[i];
                task->result = resultsArray;
                task->index = i;
                task->inputArray = inputArray; // Passa o vetor de entrada
                task->nTotalElements = nTotalElements;     // Passa o número de elementos

                pthread_create(&Thread[i], NULL, bsearch_lower_bound_task, &my_thread_id[i]);
        }

        // Medindo tempo SEM criacao das threads
        pthread_barrier_wait(&myBarrier);        // que acontece se isso for omitido ?        
        chrono_start(&parallelReductionTime);

        bsearch_lower_bound_task(&my_thread_id[0]); // main faz papel da thread 0
        // chegando aqui todas as threads sincronizaram, até a 0 (main)


        // Measuring time after threads finished...
        chrono_stop(&parallelReductionTime);

        // main imprime o resultado global

        for (i = 1; i < nThreads; i++)
                pthread_join(Thread[i], NULL);   // isso é necessário ?


        pthread_barrier_destroy(&myBarrier);

        chrono_reportTime(&parallelReductionTime, "parallelReductionTime");

        // calcular e imprimir a VAZAO (numero de operacoes/s)
        double total_time_in_seconds = (double) chrono_gettotal(&parallelReductionTime) / ((double)1000*1000*1000);
        printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);

        double OPS = (nTotalElements)/total_time_in_seconds;
        printf("Throughput: %lf OP/s\n", OPS);

        return 0;
}
