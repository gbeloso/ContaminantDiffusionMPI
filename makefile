all:
	rm -rf mpi
	rm -rf seq
	rm -rf results
	mkdir results
	mkdir results/mpi
	mkdir results/seq
	mkdir results/mpi/diff
	mkdir results/mpi/matrix
	mkdir results/seq/matrix
	mkdir results/seq/diff
	mpicc -fopenmp -o mpi mpi.c -lm
	mpicc -fopenmp -o mpiG mpiGather.c -lm
	mpicc -o seq sequencial.c
run:
	./test_seq_segundo.sh
	./test_mpi.sh
clean:
	rm -rf mpi
	rm -rf seq
	rm -rf results
	mkdir results
	mkdir results/mpi
	mkdir results/seq
	mkdir results/mpi/diff
	mkdir results/mpi/matrix
	mkdir results/seq/matrix
	mkdir results/seq/diff
