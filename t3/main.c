#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#include "chrono.h"
#include "verifica.h"

long long nTotalElements;

long long geraAleatorioLL() {

        int a = rand();  // Retorna um inteiro pseudo-aleatório entre 0 e RAND_MAX.
        int b = rand();  // O mesmo acima.
        long long v = (long long)a * 100 + b;

        return v;
}

// Função de comparação para qsort
int cmpLongLong(const void *a, const void *b) {

        long long x = *(const long long *)a;
        long long y = *(const long long *)b;

        return (x > y) - (x < y);
}

// Implementação de busca binária para encontrar a partição correta
int binarySearch(long long *p, int np, long long value) {

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

void preencheVetorAleatoriamente(long long *arr, int n) {

        for (int i = 0; i < n; i ++) {
                arr[i] = geraAleatorioLL();
        }
}

void imprimeVetorLongLong(long long *arr, int n) {

        for (int i = 0; i < n; i ++) {
                printf("%lld ", arr[i]);
        }
        printf("\n");
}

void imprimeVetorInt(int *arr, int n) {

        for (int i = 0; i < n; i ++) {
                printf("%d ", arr[i]);
        }
        printf("\n");
}

void multi_partition_mpi(long long *Input, int n, long long *P, int np, long long *Output, int *nO) {

        int *count_p = calloc(np, sizeof(int));

        // Conta quantos valores estão em cada faixa
        for (int i = 0; i < n; i++) {
                int curr_pos = binarySearch(P, np, Input[i]);
                count_p[curr_pos]++;
        }

        int *insert_pos = malloc(sizeof(int) * np);

        nO[0] = 0;
        int pos = 0;
        for (int i = 0; i < np - 1; i++) {
                pos += count_p[i];
                nO[i + 1] = pos;
        }

        for (int i = 0; i < np; i++) {
                insert_pos[i] = nO[i];
        }

        for (int i = 0; i < n; i++) {
                int part = binarySearch(P, np, Input[i]);
                Output[insert_pos[part]++] = Input[i];
        }
}


int main(int argc, char *argv[]) {

        if (argc != 3) {
                printf("Uso: %s <nTotalElements> <nProcMPI>\n", argv[0]);
                return EXIT_FAILURE;
        }

        int rank, size;
        MPI_Init(&argc, &argv);

        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        //printf("MPI - Processo %d de %d\n", rank, size);

        int s = 2024 * 100 + rank;
        srand(s);

        nTotalElements = atoi(argv[1]);
        int nProcMPI = atoi(argv[2]);

        int np = nProcMPI;
        int n = nTotalElements / np;

        //printf("nTotalElements = %lld\n", nTotalElements);
        //printf("nProcMPI = %d\n", nProcMPI);
        //printf("n = %d\n", n);

        // Alocação de memória
        long long *Input = malloc(sizeof(long long) * n);
        long long *P = malloc(sizeof(long long) * np);
        long long *Output = malloc(sizeof(long long) * n);
        int *nO = malloc(sizeof(int) * np);

        // Verificação de erro na alocação de memória
        if (!Input || !P || !Output || !nO) {
                printf("Erro ao alocar memória.\n");
                return EXIT_FAILURE;
        }

        // Preenchimento aleatório dos vetores Input e P
        preencheVetorAleatoriamente(Input, n);
        preencheVetorAleatoriamente(P, np - 1);
        P[np - 1] = LLONG_MAX;

        // Ordenação do vetor de partição
        qsort(P, np, sizeof(long long), cmpLongLong);

        multi_partition_mpi(Input, n, P, np, Output, nO);
        //verifica_particoes(Input, n, P, np, Output, nO);

        free(Input);
        free(P);
        free(Output);
        free(nO);

        free(sendcounts);
        free(recvcounts);
        free(sdispls);
        free(rdispls);

        MPI_Finalize();

        return EXIT_SUCCESS;
}
