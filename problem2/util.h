#include <pthread.h>
#include <sys/shm.h>
#include <stdio.h>

#define BUFSIZE 2
#define ITERATIONS 10
#define DEBUG

typedef enum color_type {
	RED, 
	BLACK, 
	WHITE
}color_t;

typedef struct item_type {
	color_t color; 	
	int timestamp;
}item_t;

typedef struct buffer_type {
	item_t buffer[BUFSIZE];
	int num_items;
	pthread_mutex_t buffer_lock;
	pthread_cond_t non_full;
	pthread_cond_t non_empty;
	int bufin;
	int bufout;
}buffer_t;



FILE *fp1, *fp2, *fp3, *fp4;

void get_item(item_t *itemp);
void put_item(item_t item);
