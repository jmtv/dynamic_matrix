#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {

    int m;              //Tamano de matriz

    int a, b, c, d;        //Numeros random

    int numprocs;       //Numero de procesos ejecutandose

    int pid;        //Numero de proceso

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Averigua el numero de procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    if (pid == 0) {

        /*Pide m al usuario*/
        printf("Ingrese un valor : ");
        scanf("%d", &m);

        /*Verifica que numprocs sea par y que m sea multiplo de numprocs*/
        if (numprocs % 2 != 0 || m % numprocs != 0) {
            printf("ERROR\nIngrese un numero de procesos par y un m que sea multiplo del numero de procesos");
        }

        /*Genera los 4 numeros random*/
        srand(time(NULL));  //Genera semilla para los random

        a = rand() % (m - 1);
        b = rand() % (m - 1);
        c = rand() % (m - 1);
        d = rand() % (m - 1);

        printf("%d %d %d %d\n", a, b, c, d);


        int *matrizA = (int *) malloc(m * m * sizeof(int));    //Asigna los espacios de la matriz A de tamano m * m


        /*Llena la matrizA con numeros random de 0 a 5 inclusivo*/
        int i = 0;
        int j = 0;
        for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                *(matrizA + (i * m) + j) = rand() % 6;
            }
        }

        /*Imprime la matrizA*/
        i = 0;
        j = 0;
        for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                printf("%d ", *(matrizA + (i * m) + j));
            }
            printf("\n");
        }

    }

    MPI_Finalize();


    return 0;
}