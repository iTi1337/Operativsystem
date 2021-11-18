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


void*
init_row(void* row){
    int i = (int*) row;
    for (int j = 0; j < SIZE; j++){
            a[i][j] = 1.0;
	        b[i][j] = 1.0;
    }
}

static void
init_matrix(void)
{
    int i, j;
/*
    for (i = 0; i < SIZE; i++)
        
        for (j = 0; j < SIZE; j++) {
	        // Simple initialization, which enables us to easy check
	        // the correct answer. Each element in c will have the same
	        // value as SIZE after the matmul operation.
	         
	        a[i][j] = 1.0;
	        b[i][j] = 1.0;
        }
    */
    pthread_t *init_rows;
    init_rows = malloc(SIZE * sizeof(pthread_t));
    int threadArgs[SIZE];
    for (int i = 0; i < SIZE; i++) {
        threadArgs[i] = i;
        pthread_create(&(init_rows[i]), NULL, init_row, (void*)threadArgs[i]);
    }
    for (int id = 0; id < SIZE; id++)
        pthread_join(init_rows[id], NULL);
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
    int threadArgs[SIZE];
    for (int i = 0; i < SIZE; i++) {
        threadArgs[i] = i;
        pthread_create(&(rows[i]), NULL, matmul_row, (void*)threadArgs[i]);
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
The original program took 2.296 seconds to run in real time.
With our changes, it took 1.517 seconds to run in real time
Our speedup was 51%

With the initialization also using threads, we got 1.741 seconds, which is a slight decrease in performance. 

We believe this is due to the low impact the initilization function has on the programs runtime, 
in comparison to the multiplication function taking up 1024 times more loops.

Since the initialization function is so small, using threads here is a waste of resources due to the large overhead, 
making the program slower overall
*/