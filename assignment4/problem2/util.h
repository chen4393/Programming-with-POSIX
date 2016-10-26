#include <pthread.h>
#include <stdio.h>

#define BUFSIZE 2
#define STRING_LEN 30
#define ITERATIONS 1000

typedef struct buffer_type
{	
	pthread_mutex_t buffer_lock;
	pthread_cond_t non_full;//the condition variable producers have to wait 
	pthread_cond_t non_empty;//the condition variable consumer has to wait
	pthread_mutexattr_t mutexattr;
	pthread_condattr_t condattr;
	int num_items;//the number of items in the shared buffer
	int bufin;//the start index producers needs to begin depositing
	int bufout;//the start index consumer needs to begin removing 
	char buffer[BUFSIZE][STRING_LEN];
}buffer_t;

