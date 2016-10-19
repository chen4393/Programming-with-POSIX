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
