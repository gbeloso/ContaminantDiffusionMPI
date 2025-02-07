#!/bin/bash

program="seq"

# Number of times to run the program
n=2048

#for ((i=64; i<=n; i*=2)); do
#  for ((j=2; j<=n; j*=2)); do
#    echo "Matriz [$n][$n] - Qtd iteracao [$j]"
#    ./$program $n $j
    #./$program $i $j
#  done
#done

for ((j=2;j<=n; j*=2));do
	echo "Matriz [$n][$n] - Qtd iteracao [$j]"
	./$program $n $j
done

echo "Finalizado Sequencial"
