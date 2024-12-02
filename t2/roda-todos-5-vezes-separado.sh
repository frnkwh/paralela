#!/bin/bash


# Esse script foi criado para rodar o roda_todos_slurm 5 vezes
# ao inves de aumentar o MAX_EXECS do proprio roda_todos_slurm,
# ja que o programa ficou um pouco lento, e entao os 5 minutos
# nao sao suficientes para rodar todos os experimentos em um 
# unico job do slurm

# Verifica se o parâmetro foi fornecido
if [ -z "$1" ]; then
  echo "USAGE: $0 <A|B>"
  exit 1
fi

# Roda o script 'roda_todos_slurm.sh' 5 vezes
for i in {1..5}
do
  # Executa o script e salva a saída em arquivos diferentes
  sbatch --exclusive ./roda-todos-slurm.sh $1 
done
