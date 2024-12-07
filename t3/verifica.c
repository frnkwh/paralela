#include "verifica.h"

// Função que verifica se o particionamento está correto
void verifica_particoes(long long *Input, int n, long long *P, int np, long long *Output, int *nO) {
        for (int i = 0; i < np; i++) {
                int start = nO[i];
                int end = (i == np - 1) ? n : nO[i + 1];
                for (int j = start; j < end; j++) {
                        // Verifica se cada elemento está na partição correta
                        if ((i == 0 && Output[j] >= P[i]) || (i > 0 && (Output[j] < P[i - 1] || Output[j] >= P[i]))) {
                                printf("Erro: Elemento %lld na nOição %d fora da faixa [%lld, %lld)\n", Output[j], j, (i > 0 ? P[i - 1] : 0), P[i]);
                                return;
                        }
                }
        }
        printf("Particionamento sem erros\n");
}
