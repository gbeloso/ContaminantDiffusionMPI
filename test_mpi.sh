#!/bin/bash

program="mpi"
threads=4  # Replace this with your actual static argument value

# Number of times to run the program
n=2048

for ((i=64; i<=n; i*=2)); do
  for ((j=2; j<=n; j*=2)); do
    ./$program $i $j $threads
  done
done
