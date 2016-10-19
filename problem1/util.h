#define BUF_SIZE 1024
#define DEBUG

typedef enum color_type {
	RED, 
	BLACK, 
	WHITE,
	EMPTY
}color_t;

typedef struct item_type {
	color_t color; 	
	int timestamp;
}item_t;

void * producer(void *);
void * consumer(void *);
void item_insert(item_t item, item_t * buffer);
item_t item_remove(item_t * buffer);
