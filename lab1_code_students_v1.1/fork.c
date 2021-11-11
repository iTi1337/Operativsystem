#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid;
    pid_t pid2;
    unsigned i;
    unsigned niterations = 1000;
    pid = fork();
    
    if (pid == 0) {
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
     } else {
        pid2 = fork();
        if (pid2 == 0){
            for (i = 0; i < niterations; ++i)
            printf("B = %d, ", i);
        } else{
            printf("Pid child 1: %d\n Pid child 2: %d\n", pid, pid2);
            for (i = 0; i < niterations; ++i)
                printf("C = %d, ", i);
        }
    }
    printf("\n");
}

/*
the fork.c
*/