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

void multi_partition_mpi(long long *Input, int n, long long *P, int np, long long *Output, int *nO) {


        // Memory allocation
        int *count_p = calloc(np, sizeof(int)); // Number of elements in each partition

        // Count how many values are in each partition
        for (int i = 0; i < n; i++) {
                int curr_pos = binarySearch(P, np, Input[i]);
                count_p[curr_pos]++;
        }

        int *insert_pos = malloc(sizeof(int) * np); // Insert positions for Output


        int *tmp_N = calloc(np, sizeof(int));

        int pos = 0;
        for (int i = 0; i < np - 1; i++) {
                pos += count_p[i];
                tmp_N[i + 1] = pos;
        }

        for (int i = 0; i < np; i++) {
                insert_pos[i] = tmp_N[i];
        }


        // Reorganize Input into Output based on partitions
        for (int i = 0; i < n; i++) {
                int part = binarySearch(P, np, Input[i]);
                Output[insert_pos[part]++] = Input[i];
        }

        // Prepare for MPI_Alltoallv
        int *sendcounts = malloc(np * sizeof(int));
        int *sdispls = malloc(np * sizeof(int));
        int *recvcounts = malloc(np * sizeof(int));
        int *rdispls = malloc(np * sizeof(int));

        // Set sendcounts and sdispls based on count_p

        for (int i = 0; i < np; i++) {
                sendcounts[i] = count_p[i];  // Number of elements to send to process i
                sdispls[i] = tmp_N[i];          // Start index of partition i in Output
        }

        // Gather recvcounts for each process
        MPI_Alltoall(sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);


        // Calculate rdispls and total receive size
        int total_recv_size = 0;
        for (int i = 0; i < np; i++) {
                rdispls[i] = total_recv_size;
                total_recv_size += recvcounts[i];
        }
        *nO = total_recv_size;

        // Allocate buffer for receiving data
        long long *recvbuf = malloc(total_recv_size * sizeof(long long));

        // Perform Alltoallv communication
        MPI_Alltoallv(Output, sendcounts, sdispls, MPI_LONG_LONG,
                      recvbuf, recvcounts, rdispls, MPI_LONG_LONG, MPI_COMM_WORLD);


        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        //printf("This is process %d:\n", rank);
        //printf("Output: ");
        //imprimeVetorLongLong(Output, n);
        //printf("P: ");
        //imprimeVetorLongLong(P, np);
        //printf("Count_P: ");
        //imprimeVetorInt(count_p, np);
        //printf("nO: %d", *nO);
        //printf("============\n");

        //// Print the received data for verification
        //printf("\n\n===>>>> Process %d received:", rank);
        //for (int i = 0; i < total_recv_size; i++) {
        //        printf(" %lld", recvbuf[i]);
        //}
        //printf("\n\n");


        // Cleanup
        free(count_p);
        free(insert_pos);
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

        // Input arguments
        long long nTotalElements = atoll(argv[1]);
        int nProcMPI = atoi(argv[2]);

        if (nProcMPI != size) {
                if (rank == 0) {
                        printf("Error: Number of processes must match nProcMPI.\n");
                }
                MPI_Finalize();
                return EXIT_FAILURE;
        }


        int s = 2024 * 100 + rank;
        srand(s);

        //if (rank == 0) {
        //        printf("Chamando multi_partition_mpi %d vezes.\n", NTIMES);
        //}


        int np = nProcMPI;
        long long n = nTotalElements / np;
        long long *Input = malloc(sizeof(long long) * n);
        long long *P = malloc(sizeof(long long) * np);
        long long *Output = malloc(sizeof(long long) * n);
        int nO;   // Starting index of each partition

        // Error checking
        if (!Input || !P || !Output) {
                printf("Erro ao alocar memória.\n");
                MPI_Finalize();
                free(Input);
                free(P);
                free(Output);
                return EXIT_FAILURE;
        }

        // Rank 0 initializes P and broadcasts it to all processes
        if (rank == 0) {
                for (int i = 0; i < np - 1; i++) {
                        P[i] = geraAleatorioLL();
                }
                P[np - 1] = LLONG_MAX;

                // Sort P
                qsort(P, np, sizeof(long long), cmpLongLong);
        }

        MPI_Bcast(P, np, MPI_LONG_LONG, 0, MPI_COMM_WORLD);


        //printf("This is process %d. Partition:\n", rank);
        //imprimeVetorLongLong(P, np);

        // Populate Input with random values
        for (int i = 0; i < n; i++) {
                Input[i] = geraAleatorioLL();
        }

        chronometer_t multi_partition_mpi_time;

        chrono_reset(&multi_partition_mpi_time);
        chrono_start(&multi_partition_mpi_time);



        for (int i = 0; i < NTIMES; i++) {
                multi_partition_mpi(Input, n, P, np, Output, &nO);
        }

        chrono_stop(&multi_partition_mpi_time);
        chrono_reportTime(&multi_partition_mpi_time, "multi_partition_mpi_time");

        verifica_particoes(Input, n, P, np, Output, &nO);

        double total_time_in_seconds = (double) chrono_gettotal(&multi_partition_mpi_time) / ((double)1000*1000*1000);

        printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
        printf("nTotalElements = %lld\n", nTotalElements);

        double OPS = ((double)nTotalElements * NTIMES) / total_time_in_seconds;
        printf("Throughput: %lf OP/s\n", OPS);

        MPI_Finalize();

        free(Input);
        free(P);
        free(Output);

        return EXIT_SUCCESS;
}
