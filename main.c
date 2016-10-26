#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "util.h"

int main(int argc, char *argv[])
{
	/* Step1: BULDING A SHARED MEMORY BUFFER */

	int shm_id;
	buffer_t* bufp;
	key_t key = 3344;

	if((shm_id = shmget(key, sizeof(buffer_t), IPC_CREAT|0666)) < 0 )
		perror("shmid error");

	if((bufp = shmat(shm_id, NULL, 0)) == (buffer_t *)-1)
		perror("shmat error");
	
	/* Step2: INITIALIZATION */
	
	//Synchronization Primitive: Mutex and Condition Variable
	pthread_mutexattr_init(&bufp->mutexattr);
	pthread_mutexattr_setpshared(&bufp->mutexattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&bufp->buffer_lock, &bufp->mutexattr);

	pthread_condattr_init(&bufp->condattr);
	pthread_condattr_setpshared(&bufp->condattr, PTHREAD_PROCESS_SHARED);
	pthread_cond_init(&bufp->non_full, &bufp->condattr);
	pthread_cond_init(&bufp->non_empty, &bufp->condattr);
	//Buffer fields
	bufp->num_items = 0;
	bufp->bufin = 0;
	bufp->bufout = 0;

	/* Step3: PROCESSES CREATION */

	int i;
	pid_t childpid;
	char keystr[10];
	sprintf (keystr, "%d", key);
	for(i = 0; i < 4; i++) {
		childpid = fork();
		if(childpid == 0) {
			switch(i) {
				case 0: execl ("./Producer", "RED", keystr, NULL);
				case 1: execl ("./Producer", "BLACK", keystr, NULL);
				case 2: execl ("./Producer", "WHITE", keystr, NULL);
				case 3: execl ("./Consumer", keystr, NULL);
			}
			exit(0);
		}
	}

	/* Wait for children to exit. */
	int status;
	pid_t pid;
	for(i = 0; i < 4; i++) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
	}
	
	/* Step4: CLEANING REDUNDANT RESOUCES */

	//child processes finished, so destroy mutexes and CVs
	pthread_mutex_destroy(&(bufp->buffer_lock));
	pthread_cond_destroy(&(bufp->non_empty));
	pthread_cond_destroy(&(bufp->non_full));
	
	//done with the program, so detach the shared segment and terminate
	shmctl(shm_id, IPC_RMID, NULL);
	
	return 0;
}



























