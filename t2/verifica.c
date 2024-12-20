#include "verifica.h"

//// Função que verifica se o particionamento está correto
//void verifica_particoes(long long *input, int n, long long *p, int np, long long *output, int *pos) {
//        for (int i = 0; i < np; i++) {
//                int start = pos[i];
//                int end = (i == np - 1) ? n : pos[i + 1];
//                for (int j = start; j < end; j++) {
//                        // Verifica se cada elemento está na partição correta
//                        if ((i == 0 && output[j] >= p[i]) || (i > 0 && (output[j] < p[i - 1] || output[j] >= p[i]))) {
//                                printf("Erro: Elemento %lld na posição %d fora da faixa [%lld, %lld)\n", output[j], j, (i > 0 ? p[i - 1] : 0), p[i]);
//                                return;
//                        }
//                }
//        }
//        printf("particionamento sem erros\n");
//}
//
//

void verifica_particoes(long long *input, int n, long long *p, int np, long long *output, int *pos) {
    for (int i = 0; i < np; i++) {
        int start = pos[i];
        int end = (i == np - 1) ? n : pos[i + 1];

        for (int j = start; j < end; j++) {
            if ((i == 0 && output[j] >= p[i]) ||
                (i > 0 && (output[j] < p[i - 1] || output[j] >= p[i]))) {
                printf("===> particionamento COM ERROS\n");
                return;
            }
        }
    }
    printf("===> particionamento CORRETO\n");
}
