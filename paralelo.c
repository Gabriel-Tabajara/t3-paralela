#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

// #define DEBUG 1            // comentar esta linha quando for medir tempo

int RESULT_TAG = 0;
int REQUEST_TAG = 1;

int *interleaving(int vetor[], int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = tam / 2;

	for (i_aux = 0; i_aux < tam; i_aux++) {
		if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2)))
		    || (i2 == tam))
			vetor_auxiliar[i_aux] = vetor[i1++];
		else
			vetor_auxiliar[i_aux] = vetor[i2++];
	}

	return vetor_auxiliar;
}

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int main(int argc, char *argv[])
{
    int my_rank;
    int proc_n;
    int tam_vetor;
    int father;
    int *vetor;
    int ARRAY_SIZE;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <ARRAY_SIZE>\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    ARRAY_SIZE = atoi(argv[1]);
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    
    int delta = ARRAY_SIZE / ((proc_n+1) / 2);
    
    if (my_rank != 0) {
        father = (my_rank - 1) / 2;

        int level = (int)floor(log2(my_rank + 1));
        
        int count = ARRAY_SIZE/pow(2,level);

        vetor = (int*) malloc(count * sizeof(int));
        
        MPI_Status status;
        MPI_Recv(vetor, count, MPI_INT, father, REQUEST_TAG, MPI_COMM_WORLD, &status);

        tam_vetor = count;
    } else {
        int i;
        vetor = (int*) malloc(ARRAY_SIZE * sizeof(int));
        
        for (i=0 ; i<ARRAY_SIZE; i++)              /* init array with worst case for sorting */
            vetor[i] = ARRAY_SIZE-i;
        
        #ifdef DEBUG
        printf("\nVetor Inicial: ");
        for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
            printf("[%03d] ", vetor[i]);
        printf("\n");
        #endif
        
        tam_vetor = ARRAY_SIZE;
    }

    if (tam_vetor <= delta) {
        bs(tam_vetor, vetor);                     /* sort array */
    } else {
        int left = 2 * my_rank + 1;
        int right = 2 * my_rank + 2;

        MPI_Send(&vetor[0], tam_vetor/2, MPI_INT, left, REQUEST_TAG, MPI_COMM_WORLD);
        MPI_Send(&vetor[tam_vetor/2], tam_vetor/2, MPI_INT, right, REQUEST_TAG, MPI_COMM_WORLD);

        MPI_Status statusL;
        MPI_Status statusR;
        MPI_Recv(vetor, tam_vetor/2, MPI_INT, left, RESULT_TAG, MPI_COMM_WORLD, &statusL);
        MPI_Recv(&vetor[tam_vetor/2], tam_vetor/2, MPI_INT, right, RESULT_TAG, MPI_COMM_WORLD, &statusR);

        vetor = interleaving(vetor, tam_vetor);
    }
     

    if (my_rank != 0) {
        MPI_Send(vetor, tam_vetor, MPI_INT, father, RESULT_TAG, MPI_COMM_WORLD);
    } else {
        #ifdef DEBUG
        int i;
        printf("\nVetor Final: ");
        for (i=0 ; i<ARRAY_SIZE; i++)                              /* print sorted array */
            printf("[%03d] ", vetor[i]);
        printf("\n");
        #endif
    }

    MPI_Finalize();

    return 0;
}