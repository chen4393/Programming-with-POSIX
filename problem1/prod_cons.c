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

	//Log Files
	fp1 = fopen("Producer_RED.txt", "w+");
	fp2 = fopen("Producer_BLACK.txt", "w+");
	fp3 = fopen("Producer_WHITE.txt", "w+");
	fp4 = fopen("Consumer.txt", "w+");
	
	//Synchronization Primitive: Semaphore
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
		usleep(1000);
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
	//shmctl(shm_id, IPC_RMID, NULL);
	
	//Close log files
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	
	return 0;
}

void * producer(void * arg) {
	int tid = *((int *)arg);
	int i;
	char prod_info[256];
	struct timeval time_prod;
	for(i = 0; i < ITERATIONS; i++) {
		if(tid == 0) {
			sem_wait(&empty);
			sem_wait(&mutex);
			/* START CRITICAL SECTION */
			gettimeofday(&time_prod, NULL);
			sprintf(prod_info, "RED %ld\n", (long)(time_prod.tv_usec));//generate the corresponding string
			item_deposit(prod_info);//deposit the item
			fprintf(fp1, "%s", prod_info);
			/* END CRITICAL SECTION */
			sem_post(&mutex);
			sem_post(&full);
		}
		else if(tid == 1) {
			sem_wait(&empty);
			sem_wait(&mutex);
			/* START CRITICAL SECTION */
			gettimeofday(&time_prod, NULL);
			sprintf(prod_info, "BLACK %ld\n", (long)(time_prod.tv_usec));//generate the corresponding string
			item_deposit(prod_info);//deposit the item
			fprintf(fp2, "%s", prod_info);
			/* END CRITICAL SECTION */
			sem_post(&mutex);
			sem_post(&full);
		}
		else {
			sem_wait(&empty);
			sem_wait(&mutex);
			/* START CRITICAL SECTION */
			gettimeofday(&time_prod, NULL);
			sprintf(prod_info, "WHITE %ld\n", (long)(time_prod.tv_usec));//generate the corresponding string
			item_deposit(prod_info);//deposit the item
			fprintf(fp3, "%s", prod_info);
			/* END CRITICAL SECTION */
			sem_post(&mutex);
			sem_post(&full);
		}
	}
	fprintf(stderr, "A producer exited!\n");
	pthread_exit(0);
}

void * consumer(void * arg) {
	int i;
	char cons_info[256];
	for(i = 0; i < 3 * ITERATIONS; i++) {
		sem_wait(&full);
		sem_wait(&mutex);
		/* START CRITICAL SECTION */
		item_remove(cons_info);
		fprintf(fp4, "%s", cons_info);
		/* END CRITICAL SECTION */
		sem_post(&mutex);
		sem_post(&empty);
	}
	fprintf(stderr, "The consumer exited!\n");
	pthread_exit(0);
}

void item_deposit(char* item_string) {
	strcpy(buffer[tail], item_string);
	tail = (tail + 1) % BUFSIZE;
	//fprintf(stderr, "Deposit one item!\n");
}

void item_remove(char* item_string) {
	strcpy(item_string, buffer[head]);
	head = (head + 1) % BUFSIZE;
	//fprintf(stderr, "Remove one item!\n");
}

