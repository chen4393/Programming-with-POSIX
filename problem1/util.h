#include <semaphore.h>
#include <stdio.h>

#define BUFSIZE 2
#define STRING_LEN 256
#define ITERATIONS 1000
//#define DEBUG

typedef enum color_type {
	RED, 
	BLACK, 
	WHITE
}color_t;

typedef struct item_type {
	color_t color; 	
	int timestamp;
}item_t;

char buffer[BUFSIZE][STRING_LEN];

int head = 0;
int tail = 0;

sem_t mutex;
sem_t empty;
sem_t full;

FILE *fp1, *fp2, *fp3, *fp4;

void * producer(void *);
void * consumer(void *);
void item_deposit(char* item_string);
void item_remove(char* item_string);
