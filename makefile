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
	mpicc -o mpi mpi.c -fopenmp
	gcc sequencial.c -o seq
run:
	./test_seq.sh
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