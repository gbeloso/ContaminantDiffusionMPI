#!/bin/bash

program="seq"

# Number of times to run the program
n=2048

for ((i=64; i<=n; i*=2)); do
  for ((j=2; j<=n; j*=2)); do
    ./$program $i $j
  done
done
