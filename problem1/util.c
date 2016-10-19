#include <stdio.h>
#include "util.h"

void * producer(void * arg) {
	printf("I am producer thread %d\n", *(int *)arg);
	return NULL;
}

void * consumer(void * arg) {
	printf("I am consumer thread %d\n", *(int *)arg);
	return NULL;
}

void item_insert(item_t item, item_t * buffer) {
	buffer[tail] = item;
	tail = (tail + 1) % 2;
}

item_t item_remove(item_t * buffer) {
	item_t item, empty_item;
	item = buffer[head];
	empty_item.color = EMPTY;
	empty_item.timestamp = 0;
	buffer[head] = empty_item;
	head = (head + 1) % 2;
	return item;
}
