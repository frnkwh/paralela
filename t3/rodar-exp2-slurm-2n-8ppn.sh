#!/bin/bash

#SBATCH --exclusive
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=8
#SBATCH --job-name=multi-parittion-mpi
echo -----------------------------------
echo -----------------------------------
echo -----------------------------------
echo ---------- ATENCAO: usar --exclusive na linha de comando
echo -----------------------------------


OUTPUT_FILE="saida_exp2-${SLURM_JOBID}.txt"

##### SBATCH --output=slurm_exp2-${SLURM_JOBID}.out

echo "$0 rodando no host " `hostname`  
echo "$0 rodando no host " `hostname` > "$OUTPUT_FILE"

echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE

# Load necessary modules (adjust paths as needed)
#module load openmpi/4.1.4

# Run the MPI program
mpirun ./multi-partition-mpi 8000000 16 \
| tee -a "$OUTPUT_FILE" | grep -oP '(?<=total_time_in_seconds: )[^ ]*'

echo "O tempo total dessa shell foi de" $SECONDS "segundos"
echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE
#imprime infos do job slurm (e.g. TEMPO até aqui no fim do job)
squeue -j $SLURM_JOBID
