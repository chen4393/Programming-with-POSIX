#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "util.h"

int main() {
	
	int shm_id;
	key_t key = 4455;
	int flag = 1023;
	//shared memory segment creation
	shm_id = shmget (key, sizeof(item_t), flag);
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

	//Synchronization Primitive
	sem_init(&mutex, 0, 1);//semaphore mutex = 1;
	sem_init(&empty, 0, BUFSIZE);//semaphore empty = N;
	sem_init(&full, 0, 0);//semaphore full = 0;
	
	
	//POSIX threads creation
	pthread_t tid[4];
	int i;
	
	for(i = 0; i < 3; i++) {
		if(pthread_create(tid + i, NULL, producer, (void *)&i) != 0) {
			perror("pthread_create failed");
			exit(3);
		}
		//usleep(10);
	}
	if(pthread_create(tid + i, NULL, consumer, (void *)&i) != 0) {
			perror("pthread_create failed");
			exit(2);
	}
	for(i = 0; i < 4; i++) {
		if(pthread_join(tid[i], NULL) != 0) {
			perror("pthread_join failed");
			exit(2);
		}
	}
	sem_destroy(&mutex); /* destroy semaphore */
	sem_destroy(&empty); /* destroy semaphore */
	sem_destroy(&full); /* destroy semaphore */
	//done with the program, so detach the shared segment and terminate
	shmctl(shm_id, IPC_RMID, NULL);
	
	return 0;
}
