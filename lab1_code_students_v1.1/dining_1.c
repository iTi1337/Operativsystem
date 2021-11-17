#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define amount 5

int spoons[amount]; //0 not currently used, 1 in use
//pthread_mutex_t lock[5] = PTHREAD_MUTEX_INITIALIZER;

int try_take_spoon(unsigned long id) {
    if(spoons[id] == 0){
        spoons[id] = 1;
        return 1;
    }
    else{
        return 0;
    }
}

void* professor(void* buf) {
    unsigned long profID = (unsigned long)buf;
    int my_spoons = 0;
    while(my_spoons != 2){
        printf("%ld is thinking\n", profID);
        sleep((rand() % 80)/10 + 2);
        if(my_spoons == 0){
            my_spoons += try_take_spoon(profID);
            if(my_spoons == 1){
                printf("%ld got left\n", profID);
                sleep((rand() % 30)/10);
            }
            else{
                printf("%ld did not get left\n", profID);
            }
        }

        if(my_spoons == 1){
            my_spoons += try_take_spoon(profID+1 % amount);
            if(my_spoons == 2){
                printf("%ld got right\n",profID);
            }
            else{
                printf("%ld did not get right\n",profID);
            }
        }
    }
    printf("%ld is eating!\n", profID);
    sleep((rand() % 100)/10 + 10);
    spoons[profID] = 0;
    spoons[profID+1] = 0;
    return NULL;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    pthread_t *professors;
    unsigned long id = 0;
    unsigned long nThreads = amount;

    professors = malloc( nThreads * sizeof(pthread_t) );
    for (id = 0; id < nThreads; id++)
        pthread_create(&(professors[id-1]), NULL, professor, (void*)id);
	for (unsigned int id = 0; id < nThreads; id++) {
		pthread_join(professors[id], NULL );
	}
    return 0;
}

/*
if all professors somehow manage to grab their left chopstick before before anyone grabs their right one, they would all enter a deadlock where noone is able to proceed since noone will drop the already picked left one.

*/