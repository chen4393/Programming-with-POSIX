#define BUF_SIZE 1024
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

void * producer(void *);
void * consumer(void *);
//void item_insert(item_t);
//void item_remove(item_t);
