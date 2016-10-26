#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "util.h"

int main(int argc, char* argv[])
{
	int shmem_id;
	buffer_t* bufp;

	int i;
	struct timeval tp;//for getting the timestamp
	char item[STRING_LEN];
	FILE* fp;

	if((shmem_id = shmget(atoi(argv[1]), 0, 0)) < 0 )
		perror("shmget for producer");
	if((bufp = shmat(shmem_id, NULL, 0)) == (buffer_t *)-1)
		perror("shmat for producer");

	if(strcmp("RED", argv[0]) == 0)
		fp = fopen("Producer_RED.txt", "w+");
	else if(strcmp("BLACK", argv[0]) == 0)
		fp = fopen("Producer_BLACK.txt", "w+");
	else if(strcmp("WHITE", argv[0]) == 0)
		fp = fopen("Producer_WHITE.txt", "w+");
	else {
		fprintf(stderr, "Invalid COLOR!\n");
		return -1;
	}
	
	for(i = 0; i < ITERATIONS; i++)
	{
		pthread_mutex_lock(&bufp->buffer_lock);
		while(bufp->num_items == BUFSIZE)
			while(pthread_cond_wait(&bufp->non_full, &bufp->buffer_lock) != 0);
		/* START CRITICAL SECTION */
		gettimeofday(&tp, NULL);
		sprintf(item, "%s %d", argv[0], (int)tp.tv_usec);
		strcpy(bufp->buffer[bufp->bufin], item);
		bufp->bufin = ((bufp->bufin + 1) % 2);
		bufp->num_items++;
		/* END CRITICAL SECTION */
		pthread_mutex_unlock(&bufp->buffer_lock);
		pthread_cond_signal(&bufp->non_empty);
		fprintf(fp ,"%s\n", item);
	}
	fclose(fp);
	
	return 0;
}
