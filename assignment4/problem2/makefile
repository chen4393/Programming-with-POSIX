all:prod_con Producer Consumer
CC=gcc

prod_con:main.c
	$(CC) -o prod_con main.c -lpthread
Producer:producer.c
	$(CC) producer.c -o Producer -lpthread
Consumer:consumer.c
	$(CC) consumer.c -o Consumer -lpthread

clean:
	rm prod_con Producer Consumer *.txt *.o
