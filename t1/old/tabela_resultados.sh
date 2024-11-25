#!/bin/bash

# Verifica se os parâmetros foram passados
if [ "$#" -lt 2 ]; then
    echo "Uso: $0 <tamanho_do_vetor> <numero_de_threads>..."
    exit 1
fi

# Imprime o cabeçalho da tabela
printf "%-10s | %-20s | %-10s | %-10s\n" "Tamanho" "Threads" "Tempo(s)" "Total OPS"
printf "------------------------------------------------------------\n"

# Loop pelos parâmetros de entrada
while (( "$#" )); do
    a=$1
    b=$2
    shift 2

    # Executa o programa `bsearch` e captura a saída
    output=$(./bsearch "$a" "$b")
    time=$(echo "$output" | sed -n '1p')
    ops_per_sec=$(echo "$output" | sed -n '2p')

    # Calcula a quantidade total de operações
    total_ops=$(echo "$time * $ops_per_sec" | bc -l)

    # Exibe os resultados formatados
    printf "%-10s | %-20s | %-10.4f | %-10.4f\n" "$a" "$b" "$time" "$total_ops"
done

