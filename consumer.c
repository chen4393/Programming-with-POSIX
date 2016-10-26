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
	char item[30];
	FILE* fp;

	if((shmem_id = shmget(atoi(argv[0]), 0, 0)) < 0 )
		perror("shmget");

	if((bufp = shmat(shmem_id, NULL, 0)) == (buffer_t *)-1)
		perror("shmat");

	fp = fopen("Consumer.txt", "w+");
	for(i = 0; i < 3 * ITERATIONS; i++)
	{
		pthread_mutex_lock(&bufp->buffer_lock);
		while(bufp->num_items == 0)
			while(pthread_cond_wait(&bufp->non_empty, &bufp->buffer_lock) != 0);
		/* START CRITICAL SECTION */
		strcpy(item, bufp->buffer[bufp->bufout]);
		bufp->bufout = ((bufp->bufout + 1) % BUFSIZE);
		bufp->num_items--;
		/* END CRITICAL SECTION */
		pthread_mutex_unlock(&bufp->buffer_lock);
		pthread_cond_signal(&bufp->non_full);
		fprintf(fp ,"%s\n", item);
	}
	fclose(fp);

	return 0;
}
