/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024 //not 1024
#define THREADS 4

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];


void*
init_row(void* row){
    int i = (int*)row;
    int start = i * SIZE/THREADS;
    int end = start + SIZE/THREADS;
    //printf("start: %d, end: %d\n", start, end); fflush(stdout);
    for (i = start; i < end; i++){
        for (int j = 0; j < SIZE; j++) {
            a[i][j] = 1.0;
	        b[i][j] = 1.0;
        }
    }
}

static void
init_matrix(void)
{
    pthread_t *init_rows;
    init_rows = malloc(THREADS * sizeof(pthread_t));
    int threadArgs[SIZE];
    for (int i = 0; i < THREADS; i++) {
        threadArgs[i] = i;
        pthread_create(&(init_rows[i]), NULL, init_row, (void*)threadArgs[i]);
    }
    for (int id = 0; id < THREADS; id++)
        pthread_join(init_rows[id], NULL);
    
}

void* 
matmul_row(void* row){
    int i = (int*)row;
    int start = i * SIZE/THREADS;
    int end = start + SIZE/THREADS;
    //printf("start: %d, end: %d\n", start, end); fflush(stdout);
    for (i = start; i < end; i++){
        for (int j = 0; j < SIZE; j++) {
            c[i][j] = 0.0;
            for (int k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void
matmul_seq()
{
    pthread_t *rows;
    rows = malloc(THREADS * sizeof(pthread_t));
    int threadArgs[SIZE];

    for (int i = 0; i < THREADS; i++) {
        threadArgs[i] = i;
        pthread_create(&(rows[i]), NULL, matmul_row, (void*)threadArgs[i]);
    }
    for (int id = 0; id < THREADS; id++)
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
The difference from the previous program is 0.479 seconds, corresponding to an increase of 38%. 
We believe this is because of the optimized usage of threads in the program, ignoring creating more threads than the processor has.

normal: 2.296
row_paralell: 1.517
init_paralell: 1.741
proc_paralell: 1.262
*/