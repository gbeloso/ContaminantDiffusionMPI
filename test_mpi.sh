#!/bin/bash

program="mpi"
program2="mpiG"
max_thread=2  # Número fixo de threads
max_num_proc=8 # Número fixo de processos MPI

# Número máximo da grade
n=2000

echo "TestesReduce = {"
for((num_procs=1; num_procs<=max_num_proc;num_procs=num_procs*2)); do
	for ((threads=1; threads<=max_thread;threads=threads*2)); do
		echo -e "\nProcs: $num_procs\t Threads: $threads"
		for ((num_iterations=0; num_iterations<=500; num_iterations = num_iterations +10)); do
			mpirun -np $num_procs ./$program $n $num_iterations $threads
		done
	done
done
echo "}"
echo -e "Finalizado MPI Reduce \n\n\n\n"

echo "TestesGather = {"
for((num_procs=1; num_procs<=max_num_proc;num_procs=num_procs*2)); do
	for ((threads=1; threads<=max_thread;threads=threads*2)); do
		echo -e "\nProcs: $num_procs\t Threads: $threads"
		for ((num_iterations=0; num_iterations<=500; num_iterations = num_iterations +10)); do
			mpirun -np $num_procs ./$program2 $n $num_iterations $threads
		done
	done
done
echo "}"
echo -e "Finalizado MPI Gather \n\n\n\n"