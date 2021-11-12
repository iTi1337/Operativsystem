#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> /* For O_* constants */

const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";

int main(int argc, char **argv)
{
	pid_t pid;
	sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 1); //open, initial value 1
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0); //closed, initial value 2
	int i, status;

	pid = fork();
	if (pid) { //parent
		for (i = 0; i < 100; i++) {
			sem_wait(sem_id1); //wait for open nr1
			putchar('A'); fflush(stdout);
			sem_post(sem_id2); //open nr2
		}
		sem_close(sem_id1);
		sem_close(sem_id2);
		wait(&status);
		sem_unlink(semName1);
		sem_unlink(semName2);
	} else {
		for (i = 0; i < 100; i++) {
			sem_wait(sem_id2); //wait for parent to post sem 2
			putchar('B'); fflush(stdout);
			sem_post(sem_id1); //opens sem 1 for parent
		}
		sem_close(sem_id1);
		sem_close(sem_id2);
	}
}
