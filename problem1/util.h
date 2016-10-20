#include <semaphore.h>
#define BUFSIZE 2
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

item_t *buffer;

int head = 0;
int tail = 0;

sem_t mutex;
sem_t empty;
sem_t full;

void * producer(void *);
void * consumer(void *);
void item_deposit(item_t item);
void item_remove(item_t * item);
