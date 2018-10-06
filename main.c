#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>


void printMatrix(int *matrixA, int lenght, int width) {
    int i, j;

    for (i = 0; i < lenght; i++) {
        for (j = 0; j < width; j++) {
            printf("%d ", *(matrixA + (i * width) + j));
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {

    int *matrixB;

    int *matrixA;

    int m;              //Tamano de matriz

    int i, j;

    int numRandom[4];        //Numeros random [a,b,c,d]

    int numProcs;       //Numero de procesos ejecutandose

    int pid;        //Numero de proceso

    double starTimeAfterInput, starTimeTotal, endTime;

    int numLineasPorProc;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numProcs); //Averigua el numero de procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    //printf("Este es mi pid: %d\n",pid);

    if (pid == 0) {
        starTimeTotal = MPI_Wtime();

        starTimeAfterInput = MPI_Wtime();

        /*Pide m al usuario*/
        printf("Ingrese un valor : ");
        scanf("%d", &m);

        /*Verifica que numProcs sea par y que m sea multiplo de numProcs*/
        if (numProcs % 2 != 0 || m % numProcs != 0 || m < numProcs) {
            printf("ERROR\nIngrese un numero de procesos par y un m que sea multiplo del numero de procesos");
            MPI_Finalize();
            return (0);
        }

        numLineasPorProc = m / numProcs; //Calcula cuantas lineas tiene cada proceso

        /*Genera los 4 numeros random*/
        srand(time(NULL));  //Genera semilla para los random

        numRandom[0] = rand() % (m - 1);
        numRandom[1] = rand() % (m - 1);
        numRandom[2] = rand() % (m - 1);
        numRandom[3] = rand() % (m - 1);

        printf("Numero de procesos que corren: %d\n", numProcs); //Imprime el numero de procesos que corren

        /*Imprime los numeros random*/
        for (i = 0; i < 4; i++) {
            printf("Numero random %d = %d\n", i, numRandom[i]);
        }

        matrixA = (int *)malloc(m * m * sizeof(int));    //Asigna los espacios de la matriz A de tamano m * m

        matrixB = (int *)calloc(9 * m, sizeof(int));

        /*Llena la matrixA con numeros random de 0 a 5 inclusivo*/
        for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                *(matrixA + (i * m) + j) = rand() % 6;
            }
        }

        /*Imprime la matrixA*/
        printf("\nMATRIZ A:\n");
        printMatrix(matrixA, m, m);
    }

    /*Envia el m a todos los procesos*/
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /*Envia los numeros random a los demas procesos*/
    MPI_Bcast(numRandom, 4, MPI_INT, 0, MPI_COMM_WORLD);

    /*Envia numLineasPorProc a los demas procesos*/
    MPI_Bcast(&numLineasPorProc, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *matrixALocal = (int *)malloc(numLineasPorProc * m *
                                      sizeof(int));    //Asigna la memoria para cada fraccion de la matriz A que le toca a los procesos

    MPI_Scatter(matrixA, numLineasPorProc * m, MPI_INT, matrixALocal, numLineasPorProc * m, MPI_INT, 0, MPI_COMM_WORLD);

    /*For anidado que recorre cada entero de la matriz local recibida*/
    for(i = 0; i < numLineasPorProc; i++){
        for(j = 0; j < m; j++){

        }
    }

    /*Aqui se debe armar la matriz e imprimir informacion relevante*/
    if (pid == 0) {

        printMatrix(matrixALocal,numLineasPorProc,m);
        printf("\n\nMatriz B:\n");
        printMatrix(matrixB, 9, m);
        endTime = MPI_Wtime();

        printf("Tiempo total en ejecucion: %f\n", endTime - starTimeTotal);

        printf("Tiempo total en ejecucion: %f  \n", endTime - starTimeAfterInput);
    }


    MPI_Finalize();


    return 0;
}