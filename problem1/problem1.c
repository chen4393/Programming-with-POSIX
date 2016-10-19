#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "util.h"



int main() {
	
	int shm_id;
	item_t *buffer;
	key_t key = 4455;
	int flag = 1023;
	//shared memory segment creation
	shm_id = shmget (key, 2 * sizeof(item_t), flag);
	if(shm_id == -1) {
		perror("shmget failed");
		exit(1);
	}
	#ifdef DEBUG
	printf("Got shmem id = %d\n", shm_id);
	#endif
    //shared memory segment attachment
	buffer = shmat(shm_id, (void *)NULL, flag);
	if (buffer == (void *) -1) {
		perror ("shmat failed");
		exit (2);
	}
	#ifdef DEBUG
	printf ("Got ptr = %p\n", buffer);
	#endif

	//POSIX threads creation
	
	//done with the program, so detach the shared segment and terminate
	shmdt((void *)buffer);
	return 0;
}
