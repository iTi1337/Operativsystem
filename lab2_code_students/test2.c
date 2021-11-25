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
// We found the process was suprisingly CPU bount too, with the cpu usage spiking to almost 100% usage during runtime, which should show that the program would have gone faster, if CPU capacity were bigger.
// 5244348 blocks where written out during the processes runtime
// memory usage was 852103168