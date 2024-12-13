#include "verifica.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

void verifica_particoes(long long *Input, int n, long long *P, int np, long long *Output, int *nO) {

        int *count_p = calloc(np, sizeof(int)); // Number of elements in each partition

        // Count how many values are in each partition
        for (int i = 0; i < n; i++) {
                int curr_pos = binarySearch(P, np, Input[i]);
                count_p[curr_pos]++;
        }

        int *pos = malloc(sizeof(int) * np);
        pos[0] = 0;

        for (int i = 1; i < np; i++) {
                pos[i] = pos[i - 1] + count_p[i - 1];
        }

        for (int i = 0; i < np; i++) {
                int start = pos[i];
                int end = (i == np - 1) ? n : pos[i + 1];
                for (int j = start; j < end; j++) {
                        // Verifica se cada elemento está na partição correta
                        if ((i == 0 && Output[j] >= P[i]) || (i > 0 && (Output[j] < P[i - 1] || Output[j] >= P[i]))) {
                                printf("Erro: Elemento %lld na posição %d fora da faixa [%lld, %lld)\n", Output[j], j, (i > 0 ? P[i - 1] : 0), P[i]);
                                return;
                        }
                }
        }
        printf("Particionamento sem erros\n");
}

/*
void verifica_particoes(long long *Input, int n, long long *P, int np, 
                        long long *Output, int *nO) {
        // Verificar se todos os elementos em Output estão dentro das faixas de P
        bool particionamento_correto = true;
        for (int i = 0; i < *nO; i++) {
                bool dentro_de_uma_faixa = false;
                for (int j = 0; j < np - 1; j++) {
                        if (Output[i] >= P[j] && Output[i] < P[j + 1]) {
                                dentro_de_uma_faixa = true;
                                break;
                        }
                }
                if (!dentro_de_uma_faixa) {
                        particionamento_correto = false;
                        break;
                }
        }

        // Verificar se os elementos estão ordenados em Output
        for (int i = 1; i < *nO; i++) {
                if (Output[i] < Output[i - 1]) {
                        particionamento_correto = false;
                        break;
                }
        }

        // Imprimir o resultado
        if (particionamento_correto) {
                printf("===> particionamento CORRETO\n");
        } else {
                printf("===> particionamento COM ERROS\n");

                // Informação adicional para depuração
                printf("Elementos no Output:\n");
                for (int i = 0; i < *nO; i++) {
                        printf("%lld ", Output[i]);
                }
                printf("\n");
        }
}
*/
