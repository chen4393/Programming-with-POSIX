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
#include <sys/time.h>
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
		usleep(100);
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

void * producer(void * arg) {
	int tid = *((int *)arg);
	//printf("I am producer thread %d\n", tid);
	int i;
	char prod_info[256];
	item_t item;
	struct timeval time_prod;
	for(i = 0; i < 1000; i++) {
		if(tid == 0) {
			sem_wait(&empty);
			sem_wait(&mutex);
			/* START CRITICAL SECTION */
			item.color = RED;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			item_deposit(item);//deposit the item
			sprintf(prod_info, "RED %d\n", item.timestamp);//generate the corresponding string
			fprintf(stderr, "producer: %s", prod_info);
			/* END CRITICAL SECTION */
			sem_post(&mutex);
			sem_post(&full);
		}
		else if(tid == 1) {
			sem_wait(&empty);
			sem_wait(&mutex);
			/* START CRITICAL SECTION */
			item.color = BLACK;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			item_deposit(item);//deposit the item
			sprintf(prod_info, "BLACK %d\n", item.timestamp);//generate the corresponding string
			fprintf(stderr, "producer: %s", prod_info);
			/* END CRITICAL SECTION */
			sem_post(&mutex);
			sem_post(&full);
		}
		else {
			sem_wait(&empty);
			sem_wait(&mutex);
			/* START CRITICAL SECTION */
			item.color = WHITE;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			item_deposit(item);//deposit the item
			sprintf(prod_info, "WHITE %d\n", item.timestamp);//generate the corresponding string
			fprintf(stderr, "producer: %s", prod_info);
			/* END CRITICAL SECTION */
			sem_post(&mutex);
			sem_post(&full);
		}
	}
	fprintf(stderr, "a producer exit!\n");
	pthread_exit(0);
}

void * consumer(void * arg) {
	//printf("I am consumer thread %d\n", *(int *)arg);
	int i;
	item_t item;
	char cons_info[256];
	for(i = 0; i < 3000; i++) {
		sem_wait(&full);
		sem_wait(&mutex);
		/* START CRITICAL SECTION */
		item_remove(&item);
		if(item.color == RED) {
			sprintf(cons_info, "RED %d\n", item.timestamp);
		}
		else if(item.color == BLACK) {
			sprintf(cons_info, "BLACK %d\n", item.timestamp);
		}
		else {
			sprintf(cons_info, "WHITE %d\n", item.timestamp);
		}
		fprintf(stderr, "consumer: %s", cons_info);
		/* END CRITICAL SECTION */
		sem_post(&mutex);
		sem_post(&empty);
	}
	pthread_exit(0);
}

void item_deposit(item_t item) {
	buffer[tail] = item;
	tail = (tail + 1) % BUFSIZE;
}

void item_remove(item_t * item) {
	*item = buffer[head];
	head = (head + 1) % BUFSIZE;
}

