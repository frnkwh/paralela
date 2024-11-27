#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INPUT_SIZE 16000000
#define N_PART 1000


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

void multi_partition(long long *input, int n, long long *p, int np, long long *output, int *pos) {
        // Inicializar o vetor pos com 0
        for (int i = 0; i < np; i++) {
                pos[i] = 0;
        }

        // Criar um contador para cada faixa
        int *faixa_count = alocarVetorInt(np);
        for (int i = 0; i < np; i++) {
                faixa_count[i] = 0;
        }

        // Contar os elementos em cada faixa usando busca binária
        for (int i = 0; i < n; i++) {
                int faixa = busca_binaria(p, np, input[i]);
                faixa_count[faixa]++;
        }

        // Calcular os índices iniciais de cada faixa
        pos[0] = 0;
        for (int i = 1; i < np; i++) {
                pos[i] = pos[i - 1] + faixa_count[i - 1];
        }

        // preencher o vetor de saída
        int faixa_index[np];
        for (int i = 0; i < np; i++) {
                faixa_index[i] = pos[i];
        }

        for (int i = 0; i < n; i++) {
                int faixa = busca_binaria(p, np, input[i]);
                output[faixa_index[faixa]++] = input[i];
        }
        
        free(faixa_count);
}


int main() {
        //long long input[14] = {8, 4, 13, 7, 11, 100, 44, 3, 7, 7, 100, 110, 46, 44};
        //{8, 4, 7, 11, 3, 7, 7, 13, 44, 46, 44, 100, 100, 110}    
        //{7,4,0,3}
        //long long p[4] = {12, 70, 90, LLONG_MAX};
        //long long output[14] = {0};
        //int pos[4] = {0}; // {0, 7, 11, 11}
        //int n = 14, np = 4;

        long long *input = alocarVetorLL(INPUT_SIZE);
        long long *p = alocarVetorLL(N_PART);
        long long *output = alocarVetorLL(INPUT_SIZE);

        int *pos = alocarVetorInt(N_PART);
        printf("vetores al\n");

        int n = INPUT_SIZE;
        int np = N_PART;
 
        for (int i = 0; i < INPUT_SIZE; i++) {
                input[i] = geraAleatorioLL();
        }
        for (int i = 0; i < N_PART; i++) {
                p[i] = geraAleatorioLL();
        }
        printf("vetores aleatorios\n");

        multi_partition(input, n, p, np, output, pos);

        // Imprimir os vetores de saída
        printf("output: ");
        for (int i = 0; i < n; i++) {
                printf("%lld ", output[i]);
        }
        printf("\n");

        printf("pos: ");
        for (int i = 0; i < np; i++) {
                printf("%d ", pos[i]);
        }
        printf("\n");

        free(input);
        free(output);
        free(p);
        free(pos);

        return 0;
}

