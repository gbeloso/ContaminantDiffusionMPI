#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int N, ITER;
#define D 0.1
#define DELTA_T 0.01
#define DELTA_X 1.0

FILE * diffOut;
FILE * matrixOut;

int main(int argc, char **argv) {
    int rank, size, baseRows, remainder;
    double * mediumDif, *matrix;
    int * sendcounts, * displs, * processNoRows;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    double start_time, end_time;

    start_time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(argc != 4){
        if(rank == 0){
            printf("mpirun -np num_procs ./mpi grid_size num_iterations num_threads_per_proc\n");
        }
        return 0;
    }

    N = atoi(argv[1]);
    ITER = atoi(argv[2]);

    omp_set_num_threads(atoi(argv[3]));

    baseRows = N/size;
    remainder = N % size;

    if(rank == 0){
        matrix = (double *)malloc(N * N * sizeof(double));
        mediumDif = (double *)malloc(size * sizeof(double));
        char pathFileMatrix[200];
        sprintf(pathFileMatrix, "results/mpi/matrix/%d_%d.csv", N, ITER);
        matrixOut = fopen(pathFileMatrix, "w+");
        char pathFileDiff[200];
        sprintf(pathFileDiff, "results/mpi/diff/%d_%d.csv", N, ITER);
        diffOut = fopen(pathFileDiff, "w+");
    }

    sendcounts = (int *)malloc(size * sizeof(int));
    displs = (int *)malloc(size * sizeof(int));
    processNoRows = (int *)malloc(size * sizeof(int));

    if(rank == 0){
        for (int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++){
                matrix[i * N + j] = 0;
            }
        }
        matrix[(N/2)*N + N/2] = 1;
    }

    for(int i = 0; i < size; i++){
        processNoRows[i] = baseRows + (i < remainder ? 1 : 0);
        sendcounts[i] = processNoRows[i] * N;
        displs[i] = (i == 0 ? 0 : displs[i-1] + sendcounts[i-1]);
    }

    double * submatrix0 = (double *)malloc((processNoRows[rank] + 2) * N * sizeof(double));
    double * submatrix1 = (double *)malloc((processNoRows[rank] + 2) * N * sizeof(double));

    for(int i = 0; i < processNoRows[rank] + 2; i ++){
        for(int j = 0; j < N; j++){
            submatrix0[i * N + j] = 0;
            submatrix1[i * N + j] = 0;
        }
    }

    MPI_Scatterv(matrix, sendcounts, displs, MPI_DOUBLE, submatrix0 + N, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for(int t = 0; t < ITER; t++){
        double diff = 0;
        int i, j;
        MPI_Barrier(MPI_COMM_WORLD);
        if(t%2 == 0){
            if(rank > 0){
                MPI_Sendrecv(submatrix0 + N, N, MPI_DOUBLE, rank - 1, 0,  submatrix0, N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            if(rank < size-1){
                MPI_Sendrecv(submatrix0 + (processNoRows[rank] * N), N, MPI_DOUBLE, rank + 1, 0,  submatrix0 + (processNoRows[rank] + 1) * N, N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            
            #pragma omp parallel default(none) private(i, j) shared(submatrix1, submatrix0, processNoRows, rank, size, N) reduction(+: diff)
            {
                #pragma omp for
                for(i = 1; i < processNoRows[rank] + 1; i++){
                    for(j = 1; j < N - 1; j++){
                        int k = i * N + j;
                        if((rank == 0 && i == 1)||(rank == size - 1 && i == processNoRows[rank])){
                            submatrix1[k] = submatrix0[k];
                        }
                        else{
                            int k_e = (i-1) * N + j;
                            int k_d = (i+1) * N + j;
                            int k_c = i * N + j - 1;
                            int k_b = i * N + j + 1;
                            submatrix1[k] = submatrix0[k] + D * DELTA_T * (submatrix0[k_d] + submatrix0[k_e] + submatrix0[k_b] + submatrix0[k_c] - 4 * submatrix0[k])/(DELTA_X*DELTA_X);
                        }
                        diff += fabs(submatrix0[k] - submatrix1[k]);
                    }
                }
            }
       
        }
        else{
            if(rank > 0){
                MPI_Sendrecv(submatrix1 + N, N, MPI_DOUBLE, rank - 1, 0,  submatrix1, N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            if(rank < size-1){
                MPI_Sendrecv(submatrix1 + (processNoRows[rank] * N), N, MPI_DOUBLE, rank + 1, 0,  submatrix1 + (processNoRows[rank] + 1) * N, N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            #pragma omp parallel default(none) private(i, j) shared(submatrix1, submatrix0, processNoRows, rank, size, N) reduction(+: diff)
            {
                #pragma omp for
                for(i = 1; i < processNoRows[rank] + 1; i++){
                    for(j = 1; j < N - 1; j++){
                        int k = i * N + j;
                        if((rank == 0 && i == 1)||(rank == size - 1 && i == processNoRows[rank])){
                            submatrix0[k] = submatrix1[k];
                        }
                        else{
                            int k_e = (i-1) * N + j;
                            int k_d = (i+1) * N + j;
                            int k_c = i * N + j - 1;
                            int k_b = i * N + j + 1;
                            submatrix0[k] = submatrix1[k] + D * DELTA_T * (submatrix1[k_d] + submatrix1[k_e] + submatrix1[k_b] + submatrix1[k_c] - 4 * submatrix1[k])/DELTA_X;
                        }
                        diff += fabs(submatrix0[k] - submatrix1[k]);
                    }
                }
            }
        }

        if(t%100 == 0){
            double global_sum=0;
            // printf("Rank %d: diff = %g\n", rank, diff);

            MPI_Reduce(&diff, &global_sum, 1, MPI_DOUBLE, MPI_SUM,0, MPI_COMM_WORLD);
            if(rank == 0){
                // printf("Iteração %d: Soma total = %g, Média = %g\n", t, global_sum, global_sum / ((N - 2) * (N - 2)));
                fprintf(diffOut, "%d,%g\n", t, global_sum/((N-2)*(N-2)));
            }
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(ITER%2 == 0){
        MPI_Gatherv(submatrix0 + N, sendcounts[rank], MPI_DOUBLE, matrix, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Gatherv(submatrix1 + N, sendcounts[rank], MPI_DOUBLE, matrix, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    if(rank == 0){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(j == N-1){ 
                    fprintf(matrixOut, "%.5f\n", matrix[i * N + j]);
                }
                else{
                    fprintf(matrixOut, "%.5f,", matrix[i * N + j]);
                }
            }
        }
        //printf("Concentração final no centro: %f\n", matrix[N*N/2 + N/2]);
	}

    if(rank == 0){
        free(matrix);
    }
    end_time = MPI_Wtime();
    if(rank == 0){
	    printf("%f, ", end_time - start_time);
    }

    free(sendcounts);
    free(displs);
    free(processNoRows);
    free(submatrix0);
    free(submatrix1);
    MPI_Finalize();
    return 0;
}
