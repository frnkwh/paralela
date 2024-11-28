#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include "chrono.h"

#define INPUT_SIZE 8000000
#define N_PART 100000
#define N_TIMES 5
#define CACHE_REPS 32

pthread_barrier_t myBarrier;  // Global barrier

// Struct to pass arguments to threads
typedef struct {
        long long *input;
        long long *p;
        long long *output;
        int *pos;
        atomic_int *faixa_index;
        int n;
        int np;
        int thread_id;
        int num_threads;
} ThreadData;

int cmp_long_long(const void *a, const void *b) {
        long long x = *(const long long *)a;
        long long y = *(const long long *)b;

        if (x < y) {
                return -1;
        } else if (x > y) {
                return 1;
        } else {
                return 0;
        }
}


int *alocarVetorInt(int size) {
        int *arr = malloc(sizeof(int) * size);
        if (!arr) {
                return NULL;
        }
        return arr;
}

long long *alocarVetorLL(int size) {
        long long *arr = malloc(sizeof(long long) * size);
        if (!arr) {
                return NULL;
        }
        return arr;
}

long long geraAleatorioLL() {
        int a = rand();  // Returns a pseudo-random integer
        //    between 0 and RAND_MAX.
        int b = rand();  // same as above
        long long v = (long long)a * 100 + b;
        return v;
}  


void verifica_particoes(long long *input, int n, long long *p, int np, long long *output, int *pos) {

        for (int i = 0; i < np; i++) {
                int start = pos[i];
                int end = (i == np - 1) ? n : pos[i + 1];

                for (int j = start; j < end; j++) {
                        if ((i == 0 && output[j] >= p[i]) || (i > 0 && (output[j] < p[i - 1] || output[j] >= p[i]))) {
                                printf("Erro: Elemento %lld na posição %d está fora da faixa [%lld, %lld)\n",
                                       output[j], j, (i > 0 ? p[i - 1] : 0), p[i]);

                                printf("===> particionamento COM ERROS\n");
                                return;
                        }

                }
        }
        printf("===> particionamento SEM ERROS\n");
}


// Função auxiliar para busca binária
int busca_binaria(long long *p, int np, long long value) {
        int inicio = 0, fim = np - 1, meio;
        while (inicio < fim) {
                meio = (inicio + fim) / 2;
                if (value < p[meio]) {
                        fim = meio;
                } else {
                        inicio = meio + 1;
                }
        }
        return inicio;
}

// Função executada por cada thread
void *partition_task(void *arg) {
        ThreadData *data = (ThreadData *)arg;
        int chunk_size = data->n / data->num_threads;
        int start = data->thread_id * chunk_size;
        int end = (data->thread_id == data->num_threads - 1) ? data->n : start + chunk_size;

        for (int i = start; i < end; i++) {
                int faixa = busca_binaria(data->p, data->np, data->input[i]);
                int idx = atomic_fetch_add(&data->faixa_index[faixa], 1);
                data->output[idx] = data->input[i];
        }

        // Wait at the barrier for other threads
        pthread_barrier_wait(&myBarrier);

        return NULL;
}

void multi_partition(long long *input, int n, long long *p, int np, long long *output, int *pos, int num_threads) {
        int *faixa_count = malloc(sizeof(int) * np);
        atomic_int *faixa_index = malloc(sizeof(atomic_int) * np);

        for (int i = 0; i < np; i++) {
                faixa_count[i] = 0;
                atomic_init(&faixa_index[i], 0);
        }

        for (int i = 0; i < n; i++) {
                int faixa = busca_binaria(p, np, input[i]);
                faixa_count[faixa]++;
        }

        pos[0] = 0;
        for (int i = 1; i < np; i++) {
                pos[i] = pos[i - 1] + faixa_count[i - 1];
                atomic_store(&faixa_index[i], pos[i]);
        }

        pthread_t threads[num_threads];
        ThreadData thread_data[num_threads];

        // Initialize the barrier to synchronize the threads
        pthread_barrier_init(&myBarrier, NULL, num_threads);


        for (int i = 0; i < num_threads; i++) {
                thread_data[i] = (ThreadData){input, p, output, pos, faixa_index, n, np, i, num_threads};
                pthread_create(&threads[i], NULL, partition_task, &thread_data[i]);
        }

        for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
        }

        // Destroy the barrier after use
        pthread_barrier_destroy(&myBarrier);

        free(faixa_count);
        free(faixa_index);
}


int main(int argc, char *argv[]) {

        srand(33);

        double meps;

        long long *input = alocarVetorLL(INPUT_SIZE);
        long long *p = alocarVetorLL(N_PART * CACHE_REPS);
        long long *output = alocarVetorLL(INPUT_SIZE);

        int *pos = alocarVetorInt(N_PART);

        int n = INPUT_SIZE;
        int np = N_PART;

        chronometer_t parallelMultiPartitionTime;

        int num_threads = atoi(argv[1]);

        for (int i = 0; i < n ; i++) {
                input[i] = geraAleatorioLL();
        }

        for (int i = 0; i < np - 1; i++) {
                p[i] = geraAleatorioLL();
        }
        p[np - 1] = LLONG_MAX;
        qsort(p, np, sizeof(long long), cmp_long_long);

        for (int i = 0; i < CACHE_REPS; i++) {
                memcpy(p + i * np, p, np * sizeof(*p));
        }

        chrono_reset(&parallelMultiPartitionTime);
        chrono_start(&parallelMultiPartitionTime);

        int shift = 0;
        long long *tmp_p = p;
        for (int i = 0; i < N_TIMES; i++) {
                multi_partition(input, n, tmp_p, np, output, pos, num_threads);
                verifica_particoes(input, n, tmp_p, np, output, pos);

                tmp_p += np;
        }

        chrono_stop(&parallelMultiPartitionTime);

        double total_time_in_seconds = (double)chrono_gettotal(&parallelMultiPartitionTime) / 1e9;
        meps = INPUT_SIZE * N_TIMES / total_time_in_seconds / 1000000;
        printf("MEPS: %lf\n", meps);

        chrono_reportTime(&parallelMultiPartitionTime, "multiPartitionTime");
        printf("%.6f\n", total_time_in_seconds);

        free(input);
        free(output);
        free(p);
        free(pos);

        return 0;
}
