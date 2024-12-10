#!/bin/bash

# Uso: ./experimentos.sh <numero_do_experimento>
if [ "$#" -ne 1 ]; then
    echo "Uso: $0 <numero_do_experimento>"
    exit 1
fi

EXPERIMENT=$1
NTIMES=10
NEXECS=1
EXECUTABLE="./multi-partition-mpi"  # Nome do executável
OUTPUT_FILE="saida_exp${EXPERIMENT}-${SLURM_JOBID}.txt"

# Configuração do experimento
case $EXPERIMENT in
    1)
        NODES=1
        TASKS_PER_NODE=8
        TOTAL_ELEMENTS=1000
        ;;
    2)
        NODES=2
        TASKS_PER_NODE=8
        TOTAL_ELEMENTS=2000
        ;;
    3)
        NODES=4
        TASKS_PER_NODE=8
        TOTAL_ELEMENTS=4000
        ;;
    *)
        echo "Experimento inválido. Escolha 1, 2 ou 3."
        exit 1
        ;;
esac

echo "$0 rodando no host " `hostname` > "$OUTPUT_FILE"
echo "SLURM_JOB_NAME: " $SLURM_JOB_NAME >> "$OUTPUT_FILE"
echo "SLURM_NODELIST: " $SLURM_NODELIST >> "$OUTPUT_FILE"
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE >> "$OUTPUT_FILE"

# Diretivas do SLURM
#SBATCH --job-name=multi_partition_experiment
#SBATCH --output=$OUTPUT_FILE
#SBATCH --error=experiment_${SLURM_JOBID}.err
#SBATCH --nodes=$NODES
#SBATCH --ntasks-per-node=$TASKS_PER_NODE
#SBATCH --time=01:00:00
#SBATCH --partition=compute

module load mpi

echo "Executando experimento $EXPERIMENT com $TOTAL_ELEMENTS elementos, $NODES nós e $TASKS_PER_NODE tarefas por nó."
echo "NTIMES = $NTIMES e NEXECS = $NEXECS"

for ((i = 1; i <= NEXECS; i++)); do
    echo "Execução $i:" >> "$OUTPUT_FILE"
    srun --mpi=pmix $EXECUTABLE $TOTAL_ELEMENTS $(($NODES * $TASKS_PER_NODE)) \
        | tee -a "$OUTPUT_FILE" | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
done

echo "Tempo total da shell: $SECONDS segundos" >> "$OUTPUT_FILE"
echo "Fim do experimento. Informações do SLURM:" >> "$OUTPUT_FILE"
squeue -j $SLURM_JOBID >> "$OUTPUT_FILE"
