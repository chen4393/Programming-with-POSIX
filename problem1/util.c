#include <stdio.h>
#include <semaphore.h>
#include <sys/time.h>
#include "util.h"

int head = 0;
int tail = 0;

void * producer(void * arg) {
	int tid = *((int *)arg);
	//printf("I am producer thread %d\n", tid);
	int i;
	char prod_info[256];
	item_t item;
	struct timeval time_prod;
	for(i = 0; i < 3; i++) {
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
			sprintf(prod_info, "BALCK %d\n", item.timestamp);//generate the corresponding string
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
	return NULL;
}

void * consumer(void * arg) {
	//printf("I am consumer thread %d\n", *(int *)arg);
	int i;
	item_t item;
	char cons_info[256];
	for(i = 0; i < 3; i++) {
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
	return NULL;
}

void item_deposit(item_t item) {
	buffer[tail] = item;
	tail = (tail + 1) % BUFSIZE;
}

void item_remove(item_t * item) {
	*item = buffer[head];
	head = (head + 1) % BUFSIZE;
}
