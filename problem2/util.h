#include <pthread.h>
#include <sys/shm.h>
#include <stdio.h>

#define BUFSIZE 2
#define STRING_LEN 256
#define ITERATIONS 1000
#define DEBUG

typedef struct buffer_type {
	int num_items;//the number of items in the shared buffer
	pthread_mutex_t buffer_lock;//the mutex lock for mutual exclusion access to the buffer
	pthread_cond_t non_full;//the condition variable producers have to wait 
	pthread_cond_t non_empty;//the condition variable consumer has to wait
	int bufin;//the start index producers needs to begin depositing
	int bufout;//the start index consumer needs to begin removing 
	char items[BUFSIZE][STRING_LEN];//the string contents of the buffer
}buffer_t;



FILE *fp1, *fp2, *fp3, *fp4;

void get_item(char* item_string);
void put_item(char* item_string);
