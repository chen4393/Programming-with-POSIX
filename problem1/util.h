#define BUF_SIZE 1024
#define DEBUG

int head = 0;
int tail = 0;

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
void item_insert(item_t, item_t *);
item_t item_remove(item_t *);
