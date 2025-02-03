#!/bin/bash

program="mpi"
threads=4  # Número fixo de threads
num_procs=3  # Número fixo de processos MPI

# Número máximo da grade
n=2048

for ((num_grid=64; num_grid<=n; num_grid*=2)); do
  for ((num_iterations=2; num_iterations<=n; num_iterations*=2)); do
    mpirun -np $num_procs ./$program $num_grid $num_iterations $threads
  done
done
