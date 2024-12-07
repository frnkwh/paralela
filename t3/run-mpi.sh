#!/bin/bash

if [ $# -lt 2 ]; then
        echo "Uso: $0 <nTotalElements> <nProcMPI>"
        exit 1
fi

mpirun -np $2 ./multi-partition-mpi $1 $2
