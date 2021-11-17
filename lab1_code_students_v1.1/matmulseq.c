/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024 //not 1024

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];


static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
	        /* Simple initialization, which enables us to easy check
	         * the correct answer. Each element in c will have the same
	         * value as SIZE after the matmul operation.
	         */
	        a[i][j] = 1.0;
	        b[i][j] = 1.0;
        }
}

void* 
matmul_row(void* row){
    int i = (int*)row;
    for (int j = 0; j < SIZE; j++) {
        c[i][j] = 0.0;
        for (int k = 0; k < SIZE; k++)
            c[i][j] = c[i][j] + a[i][k] * b[k][j];
    }
}

static void
matmul_seq()
{
    pthread_t *rows;
    rows = malloc(SIZE * sizeof(pthread_t));

    for (int i = 0; i < SIZE; i++) {
        pthread_create(&(rows[i]), NULL, matmul_row, (void*)i);
    }
    for (int id = 0; id < SIZE; id++)
        pthread_join(rows[id], NULL);
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
	        printf(" %7.2f", c[i][j]);
	    printf("\n");
    }
}

int
main(int argc, char **argv)
{
    init_matrix();
    matmul_seq();
    //print_matrix();
}

/*
The original program took 7.337 seconds to run in real time.
With our changes, it took 6.440 seconds to run in real time
Our speedup was 14%
*/