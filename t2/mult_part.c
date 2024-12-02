#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include "chrono.h"

// Definição de constantes do programa
#define INPUT_SIZE 8000000    // Tamanho do array de entrada
#define CACHE_REPS 32        // Número de repetições para cache
#define MAX_THREADS 64       // Número máximo de threads permitidas
#define NTIMES 10           // Número de vezes que o particionamento será executado

// Enum para definir o tipo de operação que as threads executarão
typedef enum {
        CONTA_ELEM_PART,  // Calcular quantidade de elementos em cada partição
        CALC_VETOR_SAIDA        // Calcular o array de saída final
} Operation;

// Estrutura que contém os dados necessários para cada thread
typedef struct {
        int thread_id;           // Identificador único da thread
        long long *input;        // Array de entrada
        int n;                   // Tamanho do array de entrada
        long long *p;           // Array de pontos de partição
        int np;                 // Número de pontos de partição
        long long *output;      // Array de saída
        int *pos;              // Posições iniciais de cada partição
        int *range_count;      // Contador de elementos em cada partição
        atomic_int *range_index; // Índices atômicos para inserção paralela
        Operation op;           // Tipo de operação a ser realizada
} ThreadData;

// Variáveis globais para threads
pthread_t threads[MAX_THREADS];
ThreadData thread_data[MAX_THREADS];
pthread_barrier_t thread_barrier;  // Barreira para sincronização das threads
int num_threads;

// Função de comparação para qsort
int cmp_long_long(const void *a, const void *b) {
        long long x = *(const long long *)a;
        long long y = *(const long long *)b;
        return (x > y) - (x < y);
}

// Gera número aleatório do tipo long long
long long geraAleatorioLL() {
        int a = rand();
        int b = rand();
        return (long long)a * 100 + b;
}

// Implementação de busca binária para encontrar a partição correta
int binary_search(long long *p, int np, long long value) {
        int start = 0, end = np - 1, mid;
        while (start < end) {
                mid = (start + end) / 2;
                if (value < p[mid]) {
                        end = mid;
                } else {
                        start = mid + 1;
                }
        }
        return start;
}

// Função que verifica se o particionamento está correto
void verifica_particoes(long long *input, int n, long long *p, int np, long long *output, int *pos) {
        for (int i = 0; i < np; i++) {
                int start = pos[i];
                int end = (i == np - 1) ? n : pos[i + 1];
                for (int j = start; j < end; j++) {
                        // Verifica se cada elemento está na partição correta
                        if ((i == 0 && output[j] >= p[i]) || (i > 0 && (output[j] < p[i - 1] || output[j] >= p[i]))) {
                                printf("Erro: Elemento %lld na posição %d fora da faixa [%lld, %lld)\n", output[j], j, (i > 0 ? p[i - 1] : 0), p[i]);
                                return;
                        }
                }
        }
        printf("Particionamento sem erros\n");
}

// Função executada por cada thread
void *thread_worker(void *arg) {
        ThreadData *data = (ThreadData *)arg;

        // Calcula o intervalo de trabalho para cada thread
        int range_per_thread = data->n / num_threads;
        int start = data->thread_id * range_per_thread;
        int end = (data->thread_id == num_threads - 1) ? data->n : (data->thread_id + 1) * range_per_thread;

        while (1) {
                pthread_barrier_wait(&thread_barrier);  // Sincroniza todas as threads

                if (data->op == CONTA_ELEM_PART) {
                        // Primeira fase: conta quantos elementos vão para cada partição
                        for (int i = start; i < end; i++) {
                                int range = binary_search(data->p, data->np, data->input[i]);
                                __atomic_fetch_add(&data->range_count[range], 1, __ATOMIC_RELAXED);
                        }
                } else if (data->op == CALC_VETOR_SAIDA) {
                        // Segunda fase: coloca os elementos em suas partições finais
                        for (int i = start; i < end; i++) {
                                int range = binary_search(data->p, data->np, data->input[i]);
                                int index = atomic_fetch_add(&data->range_index[range], 1);
                                data->output[index] = data->input[i];
                        }
                }

                pthread_barrier_wait(&thread_barrier);  // Sincroniza novamente
                if (data->thread_id == 0) return NULL;
        }
        return NULL;
}

// Função principal de particionamento paralelo
void multi_partition(long long *input, int n, long long *p, int np, long long *output, int *pos) {
        static int initialized = 0;

        int range_count[np];          // Contador de elementos por partição
        atomic_int range_index[np];   // Índices atômicos para inserção paralela

        // Inicializa as threads apenas na primeira chamada
        if (!initialized) {
                pthread_barrier_init(&thread_barrier, NULL, num_threads);

                for (int i = 1; i < num_threads; i++) {
                        thread_data[i] = (ThreadData){i, input, n, p, np, output, pos, range_count, range_index, CONTA_ELEM_PART};
                        pthread_create(&threads[i], NULL, thread_worker, &thread_data[i]);
                }
                initialized = 1;
        } else {
                // Atualiza dados das threads nas chamadas subsequentes
                for (int i = 1; i < num_threads; i++) {
                        thread_data[i] = (ThreadData){i, input, n, p, np, output, pos, range_count, range_index, CONTA_ELEM_PART};
                }
        }

        // Inicializa contadores
        for (int i = 0; i < np; i++) {
                range_count[i] = 0;
                atomic_init(&range_index[i], 0);
        }

        // Configura e executa a thread principal
        thread_data[0] = (ThreadData){0, input, n, p, np, output, pos, range_count, range_index, CONTA_ELEM_PART};
        thread_worker(&thread_data[0]);

        // Calcula posições iniciais de cada partição
        pos[0] = 0;
        for (int i = 1; i < np; i++) {
                pos[i] = pos[i - 1] + range_count[i - 1];
        }

        // Inicializa índices para a fase de saída
        for (int i = 0; i < np; i++) {
                atomic_store(&range_index[i], pos[i]);
        }

        // Configura threads para a fase de saída
        for (int i = 0; i < num_threads; i++) {
                thread_data[i].op = CALC_VETOR_SAIDA;
        }
        thread_worker(&thread_data[0]);
}

// Função principal
int main(int argc, char *argv[]) {

        if (argc != 3) {
                printf("Usage: %s <A|B> <num_threads>\n", argv[0]);
                return EXIT_FAILURE;
        }


        // Configura número de threads
        num_threads = atoi(argv[2]);
        if (num_threads <= 0 || num_threads > MAX_THREADS) {
                printf("Invalid number of threads. Use between 1 and %d.\n", MAX_THREADS);
                return EXIT_FAILURE;
        }
        int n_part;

        // Define o valor de n_part com base no primeiro argumento
        if (argv[1][0] == 'A') {
                n_part = 1000;
        } else if (argv[1][0] == 'B') {
                n_part = 100000;
        } else {
                printf("Invalid partition option. Use 'A' or 'B'.\n");
                return EXIT_FAILURE;
        }

        srand(33); // Inicializa gerador de números aleatórios

        // Aloca memória para os arrays
        long long *input = malloc(sizeof(long long) * INPUT_SIZE);
        long long *p = malloc(sizeof(long long) * n_part * CACHE_REPS);
        long long *output = malloc(sizeof(long long) * INPUT_SIZE);
        int *pos = malloc(sizeof(int) * n_part);

        if (!input || !p || !output || !pos) {
                printf("Memory allocation failed.\n");
                return EXIT_FAILURE;
        }

        int n = INPUT_SIZE;
        int np = n_part;


        // Inicializa arrays com dados aleatórios
        for (int i = 0; i < INPUT_SIZE; i++) input[i] = geraAleatorioLL();
        for (int i = 0; i < n_part - 1; i++) p[i] = geraAleatorioLL();
        p[n_part - 1] = LLONG_MAX;

        // Ordena pontos de partição
        qsort(p, n_part, sizeof(long long), cmp_long_long);

        // Replica pontos de partição para cache
        for (int i = 0; i < CACHE_REPS; i++) {
                memcpy(p + i * np, p, np * sizeof(*p));
        }

        chronometer_t parallelMultiPartitionTime;

        // Mede tempo de execução
        chrono_reset(&parallelMultiPartitionTime);
        chrono_start(&parallelMultiPartitionTime);

        // Executa particionamento múltiplas vezes
        long long *tmp_p = p;
        for (int i = 0; i < NTIMES; i++) {
                multi_partition(input, n, tmp_p, np, output, pos);
                tmp_p += np;
        }

        chrono_stop(&parallelMultiPartitionTime);
        double total_time_in_seconds = (double)chrono_gettotal(&parallelMultiPartitionTime) / 1e9;

        // Verifica resultados e imprime tempo
        verifica_particoes(input, n, p, np, output, pos);
        chrono_reportTime(&parallelMultiPartitionTime, "multiPartitionTime");

    	printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);

        // Libera memória alocada
        free(input);
        free(p);
        free(output);
        free(pos);

        return 0;
}
