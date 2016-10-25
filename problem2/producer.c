#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include "util.h"

buffer_t *bufp;       /* pointer to shared memory */

int main(int argc, char* argv[]) {
	int shm_id;         /* shared memory identifier */
	shm_id = shmget(atoi(argv[1]), 0, 0);
	if (shm_id == -1) {
		perror ("Producer shmget failed");
		exit (1);
	}
	#ifdef DEBUG
	printf("Producer Got shmem id = %d\n", shm_id);
	#endif
	bufp = (buffer_t *)shmat (shm_id, (void *) NULL, 0);
    if (bufp == (void *) -1) {
		perror ("Producer shmat failed");
		exit (2);
	}
	fprintf(stderr, "Producer %s Got bufp = %p, %s\n", argv[0], bufp, bufp->log);
	
	int rtn;
	if ((rtn = pthread_mutex_lock(&bufp->buffer_lock)) != 0)
		fprintf(stderr,"%s: pthread_mutex_lock %s",argv[0], strerror(rtn)),exit(1);
	if ((rtn = pthread_mutex_unlock(&bufp->buffer_lock)) != 0)
		fprintf(stderr,"%s: pthread_mutex_unlock %s",argv[0], strerror(rtn)),exit(1);
	if ((rtn = pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock)) != 0)
		fprintf(stderr,"%s: pthread_cond_wait %s",argv[0], strerror(rtn)),exit(1);
	
	if(strcmp("RED", argv[0]) == 0)
		fp1 = fopen("Producer_RED.txt", "w+");
	else if(strcmp("BLACK", argv[0]) == 0)
		fp1 = fopen("Producer_BLACK.txt", "w+");
	else if(strcmp("WHITE", argv[0]) == 0)
		fp1 = fopen("Producer_WHITE.txt", "w+");
	else {
		fprintf(stderr, "Invalid COLOR!\n");
		return -1;
	}
	int i;
	char prod_info[STRING_LEN];		//string item to be deposited
	struct timeval time_prod;
	for(i = 0; i < ITERATIONS; i++) {
			pthread_mutex_lock(&(bufp->buffer_lock));
			while(bufp->num_items != 0)
				while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
			/* START CRITICAL SECTION */
			gettimeofday(&time_prod, NULL);
			sprintf(prod_info, "%s %d\n", argv[0], (int)time_prod.tv_usec);//generate the corresponding string
			put_item(prod_info);
			fprintf(fp1, "%s", prod_info);
			/* END CRITICAL SECTION */
			pthread_cond_signal(&bufp->non_empty);
			pthread_mutex_unlock(&bufp->buffer_lock);
	}
	
	fclose(fp1);

	return 0;
}

//Put item into  buffer at position bufin and update bufin.
void put_item(char* item_string) {
	fprintf(stderr, "before insert\n");
	strcpy(bufp->items[bufp->bufin], item_string);
	bufp->bufin = (bufp->bufin + 1) % BUFSIZE;
	bufp->num_items++;
	fprintf(stderr, "after insert\n");
	fprintf(stderr, "put one item!\n");
	return;
}

