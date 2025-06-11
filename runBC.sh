#!/bin/bash

# Default number of processes
NUM_PROCESSES=${1:-3}
ARRAY_SIZE=${2:-100}

# Compile balanceamentoCarga.c
mpicc balanceamentoCarga.c -o balanceamentoCarga -lm && \

# Run the compiled program with mpirun
mpirun -np $NUM_PROCESSES ./balanceamentoCarga $ARRAY_SIZE