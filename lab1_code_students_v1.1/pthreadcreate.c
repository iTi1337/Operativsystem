#include <stdio.h>
#include <pthread.h> // pthread types and functions

void* child() {
	printf("This is the child thread.\n");
}

int main(int argc, char** argv) {
	pthread_t thread; // struct for child-thread info
	// spawn thread:
	pthread_create(&thread, // the handle for it
		NULL,  // its attributes
		child, // the function it should run
		NULL); // args to that function

	printf("This is the parent (main) thread.\n");
	pthread_join(thread, NULL); // wait for child to finish
	return 0;
}
/*
The program prints:
	This is the parent (main) thread.
	This is the child thread.
then the parent waits for the child to return after the child() function is exectued before closing the main(), to make sure the child is finnished before exiting.
*/