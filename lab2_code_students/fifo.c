#include <stdio.h>
#include <stdlib.h>

int page_size = 256;
int no_phys_pages = 4;
FILE *fptr;
//int num;
int *pages_loaded;
int main(int argc, char *argv[]){
    if (argc < 4){
        printf("Too few arguments!\n");
        exit(-1);
    }
    no_phys_pages = atoi(argv[1]);
    page_size = atoi(argv[2]);
    fptr = fopen(argv[3],"r");
    pages_loaded = malloc(no_phys_pages * sizeof(int));
    int buffer;
    while (fscanf(fptr, "r", buffer) == 1) // expect 1 successful conversion
    {
        printf("page_size =%d\n",page_size);
        printf("Value of n=%d on page %d\n",buffer , (int)((buffer - (buffer % page_size))/ page_size));
        if (feof(fptr)) 
        {
            printf("lmao we coding");
        }
    }


    fclose(fptr); 

    
}