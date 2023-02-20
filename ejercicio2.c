#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int numProcesadores, rango;
    int *matriz = NULL;
    int *matrizLocal = NULL;
    int i, j;
    int sumaDiagonal, valorMaximo, sumaLocal, valorMaxLocal;
    int valorMaxGlobal = -1;
    int diagonalMaxGlobal = -1;
    int n;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesadores);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);

    if (rango == 0) {
        printf("Ingrese el tamaño de la matriz: ");
        fflush(stdout);
        scanf("%d", &n);
        matriz = (int*) malloc(n*n * sizeof(int));
        for (i = 0; i < n*n; i++) {
            matriz[i] = rand() % 100;
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    matrizLocal = (int*) malloc((n/numProcesadores)*n * sizeof(int));
    MPI_Scatter(matriz, (n/numProcesadores)*n, MPI_INT, matrizLocal, (n/numProcesadores)*n, MPI_INT, 0, MPI_COMM_WORLD);

    sumaLocal = 0;
    valorMaxLocal = -1;

    for (i = 0; i < n/numProcesadores; i++) {
        for (j = 0; j < n; j++) {
            if (i == j) {
                sumaLocal += matrizLocal[i*n+j];
                if (matrizLocal[i*n+j] > valorMaxLocal) {
                    valorMaxLocal = matrizLocal[i*n+j];
                }
            }
            if (i+j == n-1) {
                sumaLocal += matrizLocal[i*n+j];
                if (matrizLocal[i*n+j] > valorMaxLocal) {
                    valorMaxLocal = matrizLocal[i*n+j];
                }
            }
        }
    }

    MPI_Reduce(&sumaLocal, &sumaDiagonal, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&valorMaxLocal, &valorMaximo, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rango == 0) {
        for (i = 0; i < numProcesadores; i++) {
            if (valorMaximo > valorMaxGlobal) {
                valorMaxGlobal = valorMaximo;
                diagonalMaxGlobal = i;
            }
        }
        printf("La suma de las diagonales es %d\n", sumaDiagonal);
        printf("El valor mas alto en las diagonales es %d\n", valorMaximo);
        printf("La diagonal con el valor mas alto es %d\n", diagonalMaxGlobal);
        if (diagonalMaxGlobal == 0 || diagonalMaxGlobal == numProcesadores-1) {
            printf("La diagonal mas grande es la diagonal principal\n");
        }else{
            printf("La diagonal mas grande es la diagonal secundaria\n");
        }
    }
    MPI_Finalize();
    return 0;
}
