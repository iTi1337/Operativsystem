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
            printf("Pid child 1: %d\nPid child 2: %d\n", pid, pid2);
            for (i = 0; i < niterations; ++i)
                printf("C = %d, ", i);
        }
    }
    printf("\n");
}

/*
The variableiis used in both processes. Is the value ofiin one process affected when the other process incrementsit? Why/Why not?

Since a fork creates a separate copy of the current environment, both processes having separate i's.
Since a fork() can't send info back, opposed to a clone().

While executing the code, we got Pid 2968 for child 1 and 2969 for child 2
*/