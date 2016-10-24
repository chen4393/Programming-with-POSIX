#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

buffer_t *bufp;       /* pointer to shared memory */

int main(int argc, char* argv[]) {
	int shm_id;         /* shared memory identifier */
	
	shm_id = shmget (atoi(argv[0]), sizeof(buffer_t), 0);
	if (shm_id == -1)
	{
		perror ("Consumer shmget failed");
		exit (1);
	}
	#ifdef DEBUG
	printf("Consumer Got shmem id = %d\n", shm_id);
	#endif
	bufp = shmat (shm_id, (void *) NULL, 0);
    if (bufp == (void *) -1)
	{
		perror ("Consumer shmat failed");
		exit (2);
	}
	fprintf (stderr, "Consumer Got bufp = %p\n", bufp);
	fprintf(stderr, "Consumer arrived!\n");
	
	fp4 = fopen("Consumer.txt", "w+");
	
	int i;
	char cons_info[STRING_LEN];
	for(i = 0; i < 3 * ITERATIONS; i++) {
		pthread_mutex_lock(&(bufp->buffer_lock));
		while(bufp->num_items == 0)
			while(pthread_cond_wait(&bufp->non_empty, &bufp->buffer_lock) != 0);
		/* START CRITICAL SECTION */
		get_item(cons_info);
		fprintf(fp4, "%s", cons_info);
		/* END CRITICAL SECTION */
		pthread_cond_signal(&bufp->non_full);
		pthread_mutex_unlock(&bufp->buffer_lock);
	}
	
	fclose(fp4);
	
	return 0;
}

//Get the next item from buffer and put it in *itemp.
void get_item(char* item_string)
{
	fprintf(stderr, "num_items = %d before remove\n", bufp->num_items);
	strcpy(item_string, bufp->items[bufp->bufout]);
	bufp->bufout = (bufp->bufout + 1) % BUFSIZE;
	bufp->num_items--;
	fprintf(stderr, "num_items = %d after remove\n", bufp->num_items);
	fprintf(stderr, "take one item!\n");
}
