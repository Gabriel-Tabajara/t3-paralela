#!/bin/bash

# Default number of processes
NUM_PROCESSES=${1:-3}
ARRAY_SIZE=${2:-100}

# Compile paralelo.c
mpicc paralelo.c -o paralelo -lm && \

# Run the compiled program with mpirun
mpirun -np $NUM_PROCESSES ./paralelo $ARRAY_SIZE