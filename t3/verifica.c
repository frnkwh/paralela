#include "verifica.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
