all: prod_cons Producer Consumer

prod_cons: main.c
	cc main.c -o prod_cons -lpthread

Producer: producer.c
	cc producer.c -o Producer

Consumer: consumer.c
	cc consumer.c -o Consumer

clean:
	rm prod_cons Producer Consumer *.o *.txt
