#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "util.h"

int main() {
	//Shared Buffer
	buffer_t *bufp;       /* pointer to shared memory */
	int shm_id;//shared memory id
	key_t key = 4400;
	//int flag = 1023;
	//shared memory segment creation
	shm_id = shmget(key, sizeof(buffer_t), IPC_CREAT | 0666);
	if(shm_id == -1) {
		perror("shmget failed");
		exit(1);
	}
	#ifdef DEBUG
	printf("Got shmem id = %d\n", shm_id);
	#endif
	//shared memory segment attachment
	bufp = (buffer_t *)shmat(shm_id, (void *)NULL, 1023);
	if (bufp == (void *) -1) {
		perror ("shmat failed");
		exit (2);
	}
	#ifdef DEBUG
	printf("Got ptr = %p\n", bufp);
	#endif

	//Synchronization Primitive: Mutex and Condition Variable
	int rtn;
	if((rtn = pthread_mutexattr_init(&bufp->mutexattr)) != 0)
		fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);
	if((rtn = pthread_mutexattr_setpshared(&bufp->mutexattr, PTHREAD_PROCESS_SHARED)) != 0)
		fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);
	if((rtn = pthread_mutex_init(&bufp->buffer_lock, &bufp->mutexattr)) != 0)
		fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);
	
	if((rtn = pthread_condattr_init(&bufp->condattr)) != 0)
		fprintf(stderr,"pthreas_condattr_init: %s",strerror(rtn)),exit(1);
	if((rtn = pthread_condattr_setpshared(&bufp->condattr, PTHREAD_PROCESS_SHARED)) != 0)
		fprintf(stderr,"pthreas_condattr_setpshared: %s",strerror(rtn)),exit(1);
	if((rtn = pthread_cond_init(&bufp->non_empty, &bufp->condattr)) != 0)
		fprintf(stderr,"pthreas_condattr_init: %s",strerror(rtn)),exit(1);
	if((rtn = pthread_cond_init(&bufp->non_full, &bufp->condattr)) != 0)
		fprintf(stderr,"pthreas_condattr_init: %s",strerror(rtn)),exit(1);
	
	bufp->num_items = 0;
	bufp->bufin = 0;
	bufp->bufout = 0;
	strcpy(bufp->log, "INFO IN SHARED MEMORY");
	//producers and consumer creation
	int i;
	pid_t childpid[4];
	char keystr[10];
	sprintf (keystr, "%d", key);
	for(i = 0; i < 4; i++) {
		childpid[i] = fork();
		if(childpid[i] == 0) {
			switch(i) {
				case 1: execl ("./1Producer", "RED", keystr, NULL);
				case 2: execl ("./1Producer", "BLACK", keystr, NULL);
				case 3: execl ("./1Producer", "WHITE", keystr, NULL);
				case 0: execl ("./2Consumer", keystr, NULL);
			}
		}
		//usleep(10);
	}
	/* Wait for children to exit. */
	int status;
	pid_t pid;
	for(i = 0; i < 4; i++) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
	}
	
	//child processes finished, so destroy mutexes and CVs
	pthread_mutex_destroy(&(bufp->buffer_lock));
	pthread_cond_destroy(&(bufp->non_empty));
	pthread_cond_destroy(&(bufp->non_full));
	
	//done with the program, so detach the shared segment and terminate
	shmctl(shm_id, IPC_RMID, NULL);

	return 0;
}
/*


*/
