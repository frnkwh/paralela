#include "verifica.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int bSearch(long long *p, int np, long long value) {

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
                int curr_pos = bSearch(P, np, Input[i]);
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
