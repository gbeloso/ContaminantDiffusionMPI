#!/bin/bash

program="mpi"
threads=4  # Número fixo de threads
num_procs=3  # Número fixo de processos MPI

# Número máximo da grade
n=2000

#for ((num_grid=64; num_grid<=n; num_grid*=2)); do
#  for ((num_iterations=2; num_iterations<=n; num_iterations*=2)); do
#    echo "Matriz [$num_grid][$num_grid] - Qtd iteracao [$num_iterations]"
#    mpirun -np $num_procs ./$program $n $num_iterations $threads
    #mpirun -np $num_procs ./$program $num_grid $num_iterations $threads
#  done
#done

for((i=0; i<=2; i=i+1))do
	echo "Teste $i de 2"
	for ((num_iterations=0; num_iterations<=500; num_iterations = num_iterations +10)); do
		echo "Matriz [$n][$n] - Qtd iteracao [$num_iterations]"
		mpirun -np $num_procs ./$program $n $num_iterations $threads
	done
done
