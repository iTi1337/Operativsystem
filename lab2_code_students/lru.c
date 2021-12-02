#include <stdio.h>
#include <stdlib.h>

int page_size = 256;
int no_phys_pages = 4;
FILE *fptr;
int num_pagefaults = 0;
int *pages_loaded;
struct page{
    int number;
    int last_used;
};
int memory_read = 0;

int main(int argc, char *argv[]){
    if (argc < 4){
        printf("Too few arguments!\n");
        exit(-1);
    }
    no_phys_pages = atoi(argv[1]);
    page_size = atoi(argv[2]);

    int size = sizeof(struct page) * no_phys_pages;
    struct page* pages = malloc(size * no_phys_pages);
    printf("%d",size);

    printf("\n--------------------\n");
    printf("No physical pages = %d, page size = %d\n", no_phys_pages, page_size);
    printf("Reading memory trace from %s\n", argv[3]);

    fptr = fopen(argv[3],"r");
    int buffer;
    while (fscanf(fptr, "%d", &buffer) == 1) // expect 1 successful conversion
    {
        memory_read += 1;
        int page_number = (int)((buffer - (buffer % page_size))/ page_size);
        int foundit = 0;
        for(int i = 0; i < size/sizeof(struct page); i++){
            pages[i].last_used += 1;
            if(pages[i].number == page_number){
                foundit = 1;
                pages[i].last_used = 0;
            }
        }
        if (foundit == 0){ //did not found it
            struct page *last = malloc(sizeof(struct page));
            for(int i = 0; i < size/sizeof(struct page); i++){
                if (pages[i].last_used > last->last_used){
                    last->last_used = pages[i].last_used;
                    last->number = i;
                }
            }
            pages[last->number].last_used = 0;
            pages[last->number].number = page_number;

            num_pagefaults++;
        }
    }
    printf("Read %d memory references => %d pagefaults\n", memory_read, num_pagefaults);
    printf("--------------------\n");
    fclose(fptr);    
}