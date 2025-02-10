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

#for ((num_grid=64; num_grid<=n; num_grid*=2)); do
#  for ((num_iterations=2; num_iterations<=n; num_iterations*=2)); do
#    echo "Matriz [$num_grid][$num_grid] - Qtd iteracao [$num_iterations]"
#    mpirun -np $num_procs ./$program $n $num_iterations $threads
    #mpirun -np $num_procs ./$program $num_grid $num_iterations $threads
#  done
#done

# for((i=0; i<=2; i=i+1))do
# 	echo "Teste $i de 2"
# 	for ((num_iterations=0; num_iterations<=500; num_iterations = num_iterations +10)); do
# 		echo "Matriz [$n][$n] - Qtd iteracao [$num_iterations]"
# 		mpirun -np $num_procs ./$program $n $num_iterations $threads
# 	done
# done