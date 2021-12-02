#include <stdio.h>
#include <stdlib.h>

int page_size = 256;
int no_phys_pages = 4;
FILE *fptr;

int main(int argc, char *argv[]){
    if (argc < 4){
        printf("Too few arguments!\n");
        exit(-1);
    }

    no_phys_pages = atoi(argv[1]);
    page_size = atoi(argv[2]);
    fptr = fopen(argv[3],"r");

    int first_added = 0;
    int num_pagefaults = 0;

    int *pages_loaded;
    int size = no_phys_pages * sizeof(int);
    pages_loaded = malloc(size);

    printf("\n--------------------\n");
    printf("No physical pages = %d, page size = %d\n", no_phys_pages, page_size);
    printf("Reading memory trace from %s\n", argv[3]);

    int memory_read = 0;
    int buffer;
    while (fscanf(fptr, "%d", &buffer) == 1) // expect 1 successful conversion
    {
        memory_read += 1;
        int page_number = (int)((buffer - (buffer % page_size))/ page_size);
        int foundit = 0;
        for(int i = 0; i < size/sizeof(int); i++){
            if((int)pages_loaded[i] == page_number){
                foundit = 1;
                break;
            }
        }
        if (foundit == 0){ //did not found it
            num_pagefaults++;
            pages_loaded[first_added] = page_number;
            first_added += 1;
            if(first_added >= no_phys_pages){
                first_added = 0;
            }
        }
    }
    printf("Read %d memory references => %d pagefaults\n", memory_read, num_pagefaults);
    printf("--------------------\n");

    fclose(fptr);
    free(pages_loaded);
}