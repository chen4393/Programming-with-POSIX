all: prod_cons 1Producer 2Consumer

prod_cons: main.c
	cc main.c -o prod_cons -lpthread

1Producer: producer.c
	cc producer.c -o 1Producer

2Consumer: consumer.c
	cc consumer.c -o 2Consumer

clean:
	rm prod_cons 1Producer 2Consumer *.o *.txt
