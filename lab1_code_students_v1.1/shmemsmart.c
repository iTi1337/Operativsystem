#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";

int main(int argc, char **argv)
{
	sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 1); //open, initial value 1
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0); //closed, initial value 2
	int status;
	struct shm_struct {
		int buffer[10];
		unsigned empty[10];
	};
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0,  shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0); //shmid is a pointer to the shared struct
	for(int i = 0; i < sizeof(shmp->empty) / sizeof(unsigned); ++i){
		shmp->empty[i] = 0;
	} // Fills the empty array with 0's
	pid = fork();
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			sem_wait(sem_id1); //wait for open nr1
			printf("Sending %d\n", var1); fflush(stdout);
			shmp->buffer[var1 % 10] = var1; //adds to the buffer in a circular way
			shmp->empty[var1 % 10] = 1; //adds to empty in a circular way
			var1++;
			double a = (rand() % 100) / 50.0; //generates a random number between 0 and 2
			sem_post(sem_id2);
			sleep(a);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
		sem_close(sem_id1);
		sem_close(sem_id2);
		wait(&status);
		sem_unlink(semName1);
		sem_unlink(semName2);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100){
			/* read from shmem */
			sem_wait(sem_id2); //wait for parent to post sem 2
			var2 = shmp->buffer[var2 % 10]; //loads from buffer in a circular way
			shmp->empty[var2 % 10] = 0; //removes from empty in a circular way
			printf("Received %d\n", var2); fflush(stdout);
			var2++;
			double a = (rand() % 100) / 50.0;
			sem_post(sem_id1); //opens sem 1 for parent
			sleep(a);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
		sem_close(sem_id1);
		sem_close(sem_id2);
		wait(&status);
		sem_unlink(semName1);
		sem_unlink(semName2);
	}
}
