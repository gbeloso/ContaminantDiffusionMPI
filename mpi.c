#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 10  // Tamanho da matriz (NxN)
#define ITER 3  // Número de iterações do stencil
#define D 0.1
#define DELTA_T 0.01
#define DELTA_X 1.0

void printMatrix(double *matrix){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.5f ", matrix[i * N + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int rank, size, baseRows, remainder;
    double * mediumDif, *matrix;
    int * sendcounts, * displs, * processNoRows;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    baseRows = N/size;
    remainder = N % size;

    if(rank == 0){
        matrix = (double *)malloc(N * N * sizeof(double));
        mediumDif = (double *)malloc(size * sizeof(double));
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
        MPI_Barrier(MPI_COMM_WORLD);
        if(t%2 == 0){
            if(rank > 0){
                MPI_Sendrecv(submatrix0 + N, N, MPI_DOUBLE, rank - 1, 0,  submatrix0, N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            if(rank < size-1){
                MPI_Sendrecv(submatrix0 + (processNoRows[rank] * N), N, MPI_DOUBLE, rank + 1, 0,  submatrix0 + (processNoRows[rank] + 1) * N, N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for(int i = 1; i < processNoRows[rank] + 1; i++){
                for(int j = 1; j < N - 1; j++){
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
        else{
            if(rank > 0){
                MPI_Sendrecv(submatrix1 + N, N, MPI_DOUBLE, rank - 1, 0,  submatrix1, N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            if(rank < size-1){
                MPI_Sendrecv(submatrix1 + (processNoRows[rank] * N), N, MPI_DOUBLE, rank + 1, 0,  submatrix1 + (processNoRows[rank] + 1) * N, N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for(int i = 1; i < processNoRows[rank] + 1; i++){
                for(int j = 1; j < N - 1; j++){
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

        if(t%100 == 0){
            MPI_Gather(&diff, 1, MPI_DOUBLE, mediumDif, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            if(rank == 0){
                double temp = 0;
                for(int i = 0; i < size; i++){
                    temp+=mediumDif[i];
                }
                printf("interacao %d - diferenca=%g\n", t, temp/((N-2)*(N-2)));
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
        printMatrix(matrix);
    }

    if(rank == 0){
        free(matrix);
    }
    free(sendcounts);
    free(displs);
    free(processNoRows);
    free(submatrix0);
    free(submatrix1);
    MPI_Finalize();
    return 0;
}
