#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
/* test2.c */

#define SIZE (16*1024)
#define ITERATIONS 5

int main(int argc, char **argv)
{
    struct link {
        int x[SIZE][SIZE]; // 4 * (16*1024)^2 = 1 073 741 824
    };
    struct link *start;
    int iteration;
    FILE *f;
    start = (struct link *) calloc(1, sizeof(struct link));
    if (!start) {
        printf("Fatal error: Can't allocate memory of %d x %d = %lu\n", SIZE, SIZE, (unsigned long)SIZE*SIZE);
        exit(-1);
    }
    for (iteration = 0; iteration < ITERATIONS; iteration++) {
        printf("test2, iteration: %d\n", iteration);
        f = fopen("/tmp/file1.txt", "w");
        fwrite(start->x, sizeof(start->x[0][0]), SIZE*SIZE, f); //start point, size of element, number of element, file to write to
        fclose(f);
    }
}
// Our CPU usage was found to be 40% during runtime, hinting at the program being I/O bound
// 5244348 blocks where written out during the processes runtime
// memory usage was 852103168