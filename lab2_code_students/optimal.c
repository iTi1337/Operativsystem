#include <stdio.h>
#include <stdlib.h>

int page_size = 256;
int no_phys_pages = 4;
FILE *fptr;

struct page{
    int furthest_used;
    int number;
};

struct dynamic{
    int *pages;
    int size;
    int used;
};

void init_array(struct dynamic *d, int size){
    d->pages = malloc(size * sizeof(int));
    d->used = 0;
    d->size = size;
}

void insert_array(struct dynamic *d, int page){
    if (d->used == d->size){
        d->size *= 2;
        d->pages = realloc(d->pages, d->size * sizeof(int));
    }
    d->pages[d->used++] = page;
}

int future_check(struct dynamic *d, int start, int page, int page_size){
    for(int i = start + 1; i < d->used; i++){
        if(page == d->pages[i] / page_size){
            //printf("%d\n", i-start);
            return (i-start);
        }
    }
    return d->used;
}

int main(int argc, char *argv[]){
    if (argc < 4){
        printf("Too few arguments!\n");
        exit(-1);
    }

    no_phys_pages = atoi(argv[1]);
    page_size = atoi(argv[2]);

    int num_pagefaults = 0;
    int size = sizeof(struct page) * no_phys_pages;

    struct dynamic *all_pages;
    all_pages = malloc(sizeof(struct dynamic));
    init_array(all_pages, 8);

    struct page* pages = malloc(size);
    for(int j = 0; j < size/sizeof(struct page); j++){
        pages[j].number = -1;
    }
    struct page *furthest = malloc(sizeof(struct page));


    printf("\n--------------------\n");
    printf("No physical pages = %d, page size = %d\n", no_phys_pages, page_size);
    printf("Reading memory trace from %s\n", argv[3]);

    fptr = fopen(argv[3],"r");

    int memory_read = 0;
    int buffer;


    while (fscanf(fptr, "%d", &buffer) == 1){
        insert_array(all_pages, buffer); //skriver in alla nummer i all_pages->pages[i] där i är positionen i filen
    }
    int i;
    for (i = 0; i < all_pages->used; i++){ //loops through all of the all_pages->pages
        int foundit = 0;
        int page_number = (int)((all_pages->pages[i])/ page_size); //should be correct kappa
        //printf("%d\n", page_number);
        //printf("%d\n", all_pages->pages[i]);
        for(int j = 0; j < no_phys_pages; j++){ //looks through current physical memmory to se if we can reuse a page
            //printf("%d %d\n", pages[j].number, page_number);
            if(pages[j].number == page_number){
                foundit = 1;
            }
        }
        if (foundit == 0){ //did not find it
            num_pagefaults += 1;
            furthest->furthest_used = 0;
            int throwaway = 0;
            for(int j = 0; j < no_phys_pages; j++){ //gets physical
                if(future_check(all_pages, i, pages[j].number, page_size) > furthest->furthest_used){ //calculates how long in the future it will be used
                    furthest->furthest_used = future_check(all_pages, i, pages[j].number, page_size); //the number of future moves it will be reused
                    furthest->number = pages[j].number;
                    throwaway = j;
                }
            }
            pages[throwaway].number = page_number;
        }
    }

    printf("Read %d memory references => %d pagefaults\n", i, num_pagefaults);
    printf("--------------------\n");

    fclose(fptr);
    free(pages);
    free(furthest);
}
