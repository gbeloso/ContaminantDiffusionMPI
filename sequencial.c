#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int N, T;
#define D 0.1 // Coeficiente de difusão
#define DELTA_T 0.01
#define DELTA_X 1.0
FILE * diffOut;
FILE * matrixOut;

void diff_eq(double **C, double **C_new) { //diff_eq(double C[N][N], double C_new[N][N]) {
    for (int t = 0; t < T; t++) {
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                C_new[i][j] = C[i][j] + D * DELTA_T * (
                (C[i+1][j] + C[i-1][j] + C[i][j+1] + C[i][j-1] - 4 * C[i][j]) / (DELTA_X * DELTA_X)
                );
            }
        }
        // Atualizar matriz para a próxima iteração
        double difmedio = 0.;
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                difmedio += fabs(C_new[i][j] - C[i][j]);
                C[i][j] = C_new[i][j];
            }
        }
        if ((t%100) == 0)
        fprintf(diffOut,"%d,%g\n", t, difmedio/((N-2)*(N-2)));
    }
}
int main(int argc, char ** argv) {
    // Concentração inicial

    if(argc != 3){
        printf("./seq grid_size num_iterations\n");
        return 0;
    }
    
    double start_time, end_time;
    start_time = MPI_Wtime();

    N = atoi(argv[1]);
    T = atoi(argv[2]);

    char pathFileMatrix[200];
    sprintf(pathFileMatrix, "results/seq/matrix/%d_%d.csv", N, T);
    matrixOut = fopen(pathFileMatrix, "w+");
    char pathFileDiff[200];
    sprintf(pathFileDiff, "results/seq/diff/%d_%d.csv", N, T);
    diffOut = fopen(pathFileDiff, "w+");

    double **C = (double **)malloc(N * sizeof(double *));
    if (C == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    for (int i = 0; i < N; i++) {
        C[i] = (double *)malloc(N * sizeof(double));
        if (C[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.;
        }
    }
    // Concentração para a próxima iteração
    double **C_new = (double **)malloc(N * sizeof(double *));
    if (C_new == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    for (int i = 0; i < N; i++) {
        C_new[i] = (double *)malloc(N * sizeof(double));
        if (C_new[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C_new[i][j] = 0.;
        }
    }
    // Inicializar uma concentração alta no centro
    C[N/2][N/2] = 1.0;
    // Executar as iterações no tempo para a equação de difusão
    diff_eq(C, C_new);

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(j == N-1){ 
                fprintf(matrixOut, "%.5f\n", C[i][j]);
            }
            else{
                fprintf(matrixOut, "%.5f,", C[i][j]);
            }
        }
    }

    end_time = MPI_Wtime();
    // Exibir resultado para verificação
    //printf("Concentração final no centro: %f\n", C[N/2][N/2]);
    printf("%f\n", end_time - start_time); 
    return 0;
}
