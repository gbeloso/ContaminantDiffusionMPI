#!/bin/bash

program="seq"

# Number of times to run the program
n=2000

for((i=0; i<=2; i=i+1));do
	echo "Teste $i de 2" 
	for ((j=0;j<=500; j = j +10));do
		echo "Matriz [$n][$n] - Qtd iteracao [$j]"
		./$program $n $j
	done
done
echo "Finalizado Sequencial"
