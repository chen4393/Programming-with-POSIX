#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/time.h>
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
	int color = atoi(argv[0]);
	//fprintf(stderr, "color is %d\n", color);
	switch(color) {
		case 1:
			fprintf(stderr, "Producer RED arrived!\n");
			fp1 = fopen("Producer_RED.txt", "w+");
			break;	
		case 2:
			fprintf(stderr, "Producer BLACK arrived!\n");
			fp2 = fopen("Producer_BLACK.txt", "w+");
			break;
		case 3:
			fprintf(stderr, "Producer WHITE arrived!\n");
			fp3 = fopen("Producer_WHITE.txt", "w+");
			break;
	}
	int i;
	char prod_info[256];
	item_t item;
	struct timeval time_prod;
	for(i = 0; i < ITERATIONS; i++) {
		
		if(color == 1) {
			
			pthread_mutex_lock(&(bufp->buffer_lock));
			//fprintf(stderr, "Producer RED grab the lock!\n");
			while(bufp->num_items == BUFSIZE)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			//fprintf(stderr, "Producer RED enter the CRITICAL SECTION!\n");
			item.color = RED;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			//fprintf(stderr, "Producer RED finish creating an item!\n");
			put_item(item);
			//fprintf(stderr, "Producer RED FINISH depositing an item!\n");
			sprintf(prod_info, "RED %d\n", item.timestamp);//generate the corresponding string
			fprintf(fp1, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_mutex_unlock(&bufp->buffer_lock);
			pthread_cond_signal(&bufp->non_empty);
		}
		else if(color == 2) {
			pthread_mutex_lock(&(bufp->buffer_lock));
			while(bufp->num_items == BUFSIZE)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			item.color = BLACK;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			put_item(item);
			sprintf(prod_info, "BLACK %d\n", item.timestamp);//generate the corresponding string
			fprintf(fp2, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_mutex_unlock(&bufp->buffer_lock);
			pthread_cond_signal(&bufp->non_empty);
		}
		else {
			pthread_mutex_lock(&(bufp->buffer_lock));
			while(bufp->num_items == BUFSIZE)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			item.color = WHITE;//generate a new item
			gettimeofday(&time_prod, NULL);
			item.timestamp = (int)time_prod.tv_usec;//record the timestamp
			put_item(item);
			sprintf(prod_info, "WHITE %d\n", item.timestamp);//generate the corresponding string
			fprintf(fp3, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_mutex_unlock(&bufp->buffer_lock);
			pthread_cond_signal(&bufp->non_empty);
		}
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
void put_item(item_t item)
{
	//fprintf(stderr, "WE ARE IN THE PUT FUNCTION!\n");
	fprintf(stderr, "bufin = %d before insert\n", bufp->bufin);
	fprintf(stderr, "num_items = %d before insert\n", bufp->num_items);
	bufp->buffer[bufp->bufin] = item;
	bufp->bufin = (bufp->bufin + 1) % BUFSIZE;
	bufp->num_items++;
	fprintf(stderr, "bufin = %d after insert\n", bufp->bufin);
	fprintf(stderr, "num_items = %d after insert\n", bufp->num_items);
	fprintf(stderr, "put one item!\n");
	return;
}

