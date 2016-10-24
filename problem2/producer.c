#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "util.h"

buffer_t *bufp;       /* pointer to shared memory */


int main(int argc, char* argv[]) {
	int shm_id;         /* shared memory identifier */
	shm_id = shmget (atoi(argv[1]), sizeof(buffer_t), 0);
	if (shm_id == -1)
	{
		perror ("Producer shmget failed");
		exit (1);
	}
	#ifdef DEBUG
	printf("Producer Got shmem id = %d\n", shm_id);
	#endif
	bufp = shmat (shm_id, (void *) NULL, 0);
    if (bufp == (void *) -1)
	{
		perror ("Producer shmat failed");
		exit (2);
	}
	fprintf(stderr, "Producer Got bufp = %p\n", bufp);
	
	//3 branches
	int color;//color flag
	if(strcmp("RED", argv[0]) == 0) {
		fprintf(stderr, "Producer RED arrived!\n");
		fp1 = fopen("Producer_RED.txt", "w+");
		color = 1;
	}
	else if(strcmp("BLACK", argv[0]) == 0) {
		fprintf(stderr, "Producer BLACK arrived!\n");
		fp2 = fopen("Producer_BLACK.txt", "w+");
		color = 2;
	}
	else if(strcmp("WHITE", argv[0]) == 0) {
		fprintf(stderr, "Producer WHITE arrived!\n");
		fp3 = fopen("Producer_WHITE.txt", "w+");
		color = 3;
	}
	else {
		fprintf(stderr, "Invalid COLOR!\n");
		return -1;
	}
	int i;
	item_t item;
	char prod_info[STRING_LEN];		//string item to be deposited
	struct timeval time_prod;
	for(i = 0; i < ITERATIONS; i++) {
		if(color == 1) {
			pthread_mutex_lock(&(bufp->buffer_lock));
			//fprintf(stderr, "Producer RED grab the lock!\n");
			while(bufp->num_items != 0)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			item.color = RED;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			
			sprintf(prod_info, "RED %d\n", item.timestamp);//generate the corresponding string
			put_item(prod_info);
			fprintf(fp1, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_cond_signal(&bufp->non_empty);
			pthread_mutex_unlock(&bufp->buffer_lock);
		}
		else if(color == 2) {
			pthread_mutex_lock(&(bufp->buffer_lock));
			while(bufp->num_items != 0)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			item.color = BLACK;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			
			sprintf(prod_info, "BLACK %d\n", item.timestamp);//generate the corresponding string
			put_item(prod_info);
			fprintf(fp2, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_cond_signal(&bufp->non_empty);
			pthread_mutex_unlock(&bufp->buffer_lock);
		}
		else {
			pthread_mutex_lock(&(bufp->buffer_lock));
			while(bufp->num_items != 0)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			item.color = WHITE;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			
			sprintf(prod_info, "WHITE %d\n", item.timestamp);//generate the corresponding string
			put_item(prod_info);
			fprintf(fp3, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_cond_signal(&bufp->non_empty);
			pthread_mutex_unlock(&bufp->buffer_lock);
		}
		//usleep(1000);
	}
	
	switch(color) {
		case 1:
			fclose(fp1);
			break;	
		case 2:
			fclose(fp2);
			break;
		case 3:
			fclose(fp3);
			break;
	}
	
	return 0;
}

//Put item into  buffer at position bufin and update bufin.
void put_item(char* item_string) {
	fprintf(stderr, "num_items = %d before insert\n", bufp->num_items);
	strcpy(bufp->items[bufp->bufin], item_string);
	bufp->bufin = (bufp->bufin + 1) % BUFSIZE;
	bufp->num_items++;
	fprintf(stderr, "num_items = %d after insert\n", bufp->num_items);
	fprintf(stderr, "put one item!\n");
	return;
}

