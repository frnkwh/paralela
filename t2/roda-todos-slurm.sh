#!/bin/bash

# O experimento é definido na linha de comando

echo "USAGE: ./rodaTodos.sh <A|B>"
echo "$0 rodando no host " `hostname`  
echo "$0 rodando no host " `hostname` > "saida_$1-${SLURM_JOBID}.txt"

echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE

NTIMES=10
echo "nt " $NTIMES
MAX_EXECS=1
echo "MAX_EXECS " $MAX_EXECS

for i in {1..8}
do
    echo "Executando $NTIMES vezes com $1 elementos e $i threads:"
    for j in $(seq 1 $NTIMES);
    do
        echo "-----------------------" >>"saida_$1-${SLURM_JOBID}.txt"
        if [ $j -le $MAX_EXECS ];
        then 
          ./multpart $1 $i | tee -a "saida_$1-${SLURM_JOBID}.txt" | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
        else
          echo "nao executado" | tee -a "saida_$1-${SLURM_JOBID}.txt" 
        fi  
    done
done
echo "O tempo total dessa shell foi de" $SECONDS "segundos"
echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE
#imprime infos do job slurm (e.g. TEMPO até aqui no fim do job)
squeue -j $SLURM_JOBID





