#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#include "chrono.h"
#include "verifica.h"

#define NTIMES 10

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

void multi_partition_mpi(int rank, int size, char *argv[]) {


        // Input arguments
        nTotalElements = atoll(argv[1]);
        //printf("nTotalElements = %lld\n", nTotalElements);
        int nProcMPI = atoi(argv[2]);

        int np = nProcMPI;
        long long n = nTotalElements / np;

        // Memory allocation
        long long *Input = malloc(sizeof(long long) * n);
        long long *P = malloc(sizeof(long long) * np);
        long long *Output = malloc(sizeof(long long) * n);
        int *nO = malloc(sizeof(int) * np);   // Starting index of each partition
        int *count_p = calloc(np, sizeof(int)); // Number of elements in each partition
        int *recv_count_p = calloc(np * np, sizeof(int));

        // Error checking
        if (!Input || !P || !Output || !nO || !count_p || !recv_count_p) {
                printf("Erro ao alocar memória.\n");
                return;
        }

        // Populate Input and P with random values

        for (int i = 0; i < n; i++) Input[i] = rand() % 1000;
        for (int i = 0; i < np - 1; i++) P[i] = rand() % 1000;
        P[np - 1] = LLONG_MAX;

        // Sort P
        qsort(P, np, sizeof(long long), cmpLongLong);




        // Conta quantos valores estão em cada faixa
        for (int i = 0; i < n; i++) {
                int curr_pos = binarySearch(P, np, Input[i]);
                count_p[curr_pos]++;
        }

        int *insert_pos = malloc(sizeof(int) * np);
        //imprimeVetorInt(count_p, np);

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

        free(insert_pos);



        // Prepare for MPI_Alltoallv
        int *sendcounts = malloc(np * sizeof(int));
        int *sdispls = malloc(np * sizeof(int));
        int *recvcounts = malloc(np * sizeof(int));
        int *rdispls = malloc(np * sizeof(int));

        // Sendcounts and sdispls based on count_p and nO
        for (int i = 0; i < np; i++) {
                sendcounts[i] = count_p[i];  // Number of elements to send to process i
                sdispls[i] = nO[i];          // Start index of partition i in Output
        }

        // Gather recvcounts for each process
        MPI_Alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);

        // Calculate rdispls and total receive size
        int total_recv_size = 0;
        for (int i = 0; i < np; i++) {
                rdispls[i] = total_recv_size;
                total_recv_size += recvcounts[i];
        }

        // Allocate buffer for receiving data
        long long *recvbuf = malloc(total_recv_size * sizeof(long long));

        // Perform Alltoallv communication
        MPI_Alltoallv(Output, sendcounts, sdispls, MPI_LONG_LONG,
                      recvbuf, recvcounts, rdispls, MPI_LONG_LONG, MPI_COMM_WORLD);



        //printf("This is process %d:\n", rank);
        //printf("Output: ");
        //imprimeVetorLongLong(Output, n);
        //printf("P: ");
        //imprimeVetorLongLong(P, np);
        //printf("Count_P: ");
        //imprimeVetorInt(count_p, np);
        //printf("nO: ");
        //imprimeVetorInt(nO, np);
        //printf("============\n");

        //// Print the received data for verification
        //printf("\n\n===>>>> Process %d received:", rank);
        //for (int i = 0; i < total_recv_size; i++) {
        //        printf(" %lld", recvbuf[i]);
        //}
        //printf("\n\n");



        // Cleanup
        free(Input);
        free(P);
        free(Output);
        free(nO);
        free(count_p);
        free(recv_count_p);
        free(sendcounts);
        free(sdispls);
        free(recvcounts);
        free(rdispls);
        free(recvbuf);

}


int main(int argc, char *argv[]) {

        if (argc != 3) {
                printf("Uso: %s <nTotalElements> <nProcMPI>\n", argv[0]);
                return EXIT_FAILURE;
        }



        MPI_Init(&argc, &argv);

        int rank, size;

        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);


        int s = 2024 * 100 + rank;
        srand(s);

        chronometer_t multi_partition_mpi_time;

        chrono_reset(&multi_partition_mpi_time);
        chrono_start(&multi_partition_mpi_time);

        printf("Chamando multi_partition_mpi %d vezes.\n", NTIMES);

        for (int i = 0; i < NTIMES; i++) {
                multi_partition_mpi(rank, size, argv);
        }
        chrono_stop(&multi_partition_mpi_time);
        chrono_reportTime(&multi_partition_mpi_time, "multi_partition_mpi_time");

        double total_time_in_seconds = (double) chrono_gettotal(
                &multi_partition_mpi_time) / ((double)1000*1000*1000);
        
        printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
        printf("nTotalElements = %lld\n", nTotalElements);

        double OPS = ((double)nTotalElements * NTIMES)/total_time_in_seconds;
        printf( "Throughput: %lf OP/s\n", OPS );

        MPI_Finalize();

        return EXIT_SUCCESS;
}
