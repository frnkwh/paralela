#include <stdio.h>
#include <limits.h>

// Função auxiliar para busca binária
int busca_binaria(long long *P, int np, long long value) {
    int inicio = 0, fim = np - 1, meio;

    while (inicio < fim) {
        meio = (inicio + fim) / 2;
        if (value < P[meio]) {
            fim = meio;
        } else {
            inicio = meio + 1;
        }
    }
    return inicio;
}

void multi_partition(long long *Input, int n, long long *P, int np, long long *Output, int *Pos) {
    // Inicializar o vetor Pos com 0
    for (int i = 0; i < np; i++) {
        Pos[i] = 0;
    }

    // Criar um contador para cada faixa
    int faixa_count[np];
    for (int i = 0; i < np; i++) {
        faixa_count[i] = 0;
    }

    // Contar os elementos em cada faixa usando busca binária
    for (int i = 0; i < n; i++) {
        int faixa = busca_binaria(P, np, Input[i]);
        faixa_count[faixa]++;
    }

    // Calcular os índices iniciais de cada faixa
    Pos[0] = 0;
    for (int i = 1; i < np; i++) {
        Pos[i] = Pos[i - 1] + faixa_count[i - 1];
    }

    // Preencher o vetor de saída
    int faixa_index[np];
    for (int i = 0; i < np; i++) {
        faixa_index[i] = Pos[i];
    }

    for (int i = 0; i < n; i++) {
        int faixa = busca_binaria(P, np, Input[i]);
        Output[faixa_index[faixa]++] = Input[i];
    }
}

int main() {
    long long Input[14] = {8, 4, 13, 7, 11, 100, 44, 3, 7, 7, 100, 110, 46, 44};
    long long P[4] = {12, 70, 90, LLONG_MAX};
    long long Output[14] = {0};
    int Pos[4] = {0};
    int n = 14, np = 4;

    multi_partition(Input, n, P, np, Output, Pos);

    // Imprimir os vetores de saída
    printf("Output: ");
    for (int i = 0; i < n; i++) {
        printf("%lld ", Output[i]);
    }
    printf("\n");

    printf("Pos: ");
    for (int i = 0; i < np; i++) {
        printf("%d ", Pos[i]);
    }
    printf("\n");

    return 0;
}

