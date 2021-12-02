#include <stdio.h>
#include <stdlib.h>
/* test1.c */

#define SIZE (32*1024)
#define ITERATIONS 10

int main(int argc, char **argv)
{
    struct link {
        int x[SIZE][SIZE];
    };
    struct link *start;
    int iteration, i, j;
    start = (struct link *) calloc(1, sizeof(struct link)); //1 * 4 * (32*1024)^2 = 420691337 = 4294967296 bytes
                                                            // 4305268736 bytes used when running the process and checking with vmstat, difference is due to other processes running at the same time
                                                            // 99.7% CPU utilization
                                                            // it's a CPU-bound program since the CPU is maxxed, and it's not writing anything
    if (!start) {
        printf("Fatal error: Can't allocate memory of %d x %d = %lu\n", SIZE, SIZE, (unsigned long)SIZE*SIZE);
        exit(-1);
    }
    for (iteration = 0; iteration < ITERATIONS; iteration++) {
        printf("test1, iteration: %d\n", iteration);
        for (i = 0; i < SIZE; i++)
            for (j = 0; j < SIZE; j++)
                start->x[i][j] = iteration;
        
    }
}
