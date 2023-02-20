#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define filas 10000
#define columnas 10000
int main(int argc, char** argv) {
    int rango, numProcesadores, numeroObjetivo;
    int *matriz = NULL;
    int contador = 0, contadorGlobal = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesadores);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    // Generar matriz aleatoria en el proceso 0
    if (rango == 0) {
        srand(time(NULL));
        matriz = (int*) malloc(filas * columnas * sizeof(int));
        for (int i = 0; i < filas * columnas; i++) {
            matriz[i] = rand() % 1000;
        }
        scanf("%d", &numeroObjetivo);
    }

    // Transmitir el número de entrada a los demás procesos
    MPI_Bcast(&numeroObjetivo, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Dividir la matriz en partes iguales y enviarlas a los procesos 1, 2 y 3
    int *local_matriz = (int*) malloc((filas/numProcesadores) * columnas * sizeof(int));
    if (rango == 0) {
        for (int i = 1; i < numProcesadores; i++) {
            MPI_Send(&matriz[(i * filas/numProcesadores) * columnas], (filas/numProcesadores) * columnas, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        for (int i = 0; i < filas/numProcesadores; i++) {
            for (int j = 0; j < columnas; j++) {
                local_matriz[i*columnas+j] = matriz[i*columnas+j];
            }
        }
    } else {
        MPI_Recv(local_matriz, (filas/numProcesadores) * columnas, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    // Buscar coincidencias en la parte de la matriz correspondiente a cada proceso
    for (int i = 0; i < (filas/numProcesadores)*columnas; i++) {
        if (local_matriz[i] == numeroObjetivo) {
            contador++;
        }
    }

    // Sumar el número de coincidencias de cada proceso y mostrar el total
    MPI_Reduce(&contador, &contadorGlobal, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rango == 0) {
        printf("Cantidad de veces que aparece el número %d en la matriz: %d\n", numeroObjetivo, contadorGlobal);
    }

    free(local_matriz);
    if (rango == 0) {
        free(matriz);
    }
    MPI_Finalize();
    return 0;

}