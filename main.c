#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

    if (pid == 0) {
        starTimeTotal = MPI_Wtime();

        /*Pide m al usuario*/
        printf("Ingrese un valor : ");
        scanf("%d", &m);

        starTimeAfterInput = MPI_Wtime();

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


        /*La fila 2 de la matriz B es la fila 0 de la matriz A*/
        for(i = 0; i < m; i++){
            *(matrixB + (m * 2) + i) = *(matrixA + i);
        }
    }

    /*Envia el m a todos los procesos*/
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /*Envia los numeros random a los demas procesos*/
    MPI_Bcast(numRandom, 4, MPI_INT, 0, MPI_COMM_WORLD);

    /*Envia numLineasPorProc a los demas procesos*/
    MPI_Bcast(&numLineasPorProc, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /*Envia numProcs a los demas procesos*/
    MPI_Bcast(&numProcs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *matrixALocal = (int *)malloc(numLineasPorProc * m *
                                      sizeof(int));    //Asigna la memoria para cada fraccion de la matriz A que le toca a los procesos

    MPI_Scatter(matrixA, numLineasPorProc * m, MPI_INT, matrixALocal, numLineasPorProc * m, MPI_INT, 0, MPI_COMM_WORLD);
     
     int *sumaFilasB6;
     sumaFilasB6= (int *)calloc(numLineasPorProc, sizeof(int));

     int *sumaB4;
     sumaB4= (int *)calloc(numLineasPorProc, sizeof(int));

    int *firstHalfCol0 = (int *)malloc(numLineasPorProc * sizeof(int));
    int *secondHalfCol0 = (int *)malloc(numLineasPorProc * sizeof(int));

    /*For anidado que recorre cada entero de la matriz local recibida*/
    int columna, fila;
    for(fila = 0; fila < numLineasPorProc; fila++){
        for(columna = 0; columna < m; columna++){
            if(columna == numRandom[1] && pid < numProcs/2){
                firstHalfCol0[fila] = *(matrixALocal + (fila * m) + columna);
            }

            if(columna == numRandom[0] && pid >= numProcs/2){
                secondHalfCol0[fila] = *(matrixALocal + (fila * m) + columna);
                printf("%d",*(secondHalfCol0 + fila));
            }
            sumaFilasB6[fila] =(*(matrixALocal + (fila  * m) +columna)) +sumaFilasB6[fila ];

      	    if(columna ==1||columna ==(m-2)){
                sumaB4[fila] =(*(matrixALocal + (fila  * m) + columna)) + sumaB4[fila ];
 	     }

        }
    }

    MPI_Gather(firstHalfCol0, numLineasPorProc, MPI_INT, matrixB, numLineasPorProc, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Gather(secondHalfCol0, numLineasPorProc, MPI_INT, matrixB + m/2, numLineasPorProc, MPI_INT, 0, MPI_COMM_WORLD);
  
    MPI_Gather(sumaFilasB6, numLineasPorProc, MPI_INT,  (matrixB + (m * 6)) , numLineasPorProc, MPI_INT, 0, MPI_COMM_WORLD);
    
    MPI_Gather(sumaB4, numLineasPorProc, MPI_INT,  (matrixB + (m * 4)) , numLineasPorProc, MPI_INT, 0, MPI_COMM_WORLD);

    /*Aqui se debe armar la matriz e imprimir informacion relevante*/
    if (pid == 0) {
        printf("Numero de procesos que corren: %d\n", numProcs); //Imprime el numero de procesos que corren

        /*Imprime los numeros random*/
        for (i = 0; i < 4; i++) {
            printf("Numero random %d = %d\n", i, numRandom[i]);
        }

        /*Imprime la matrixB*/
        printf("\n\nMatriz B:\n");
        printMatrix(matrixB, 9, m);

        /*Imprime los tiempos de ejecucion*/
        endTime = MPI_Wtime();
        printf("Tiempo total en ejecucion: %f\n", endTime - starTimeTotal);
        printf("Tiempo total en ejecucion despues de input de usuario: %f  \n", endTime - starTimeAfterInput);
    }


    MPI_Finalize();


    return 0;
}
