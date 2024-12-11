#ifndef VERIFICA_H
#define VERIFICA_H

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

// Função que verifica se o particionamento está correto
void verifica_particoes(long long *Input, int n, long long *P, int np, long long *Output, int *nO);

#endif
