# ECE254 lab4 Linux Port
# Makefile: build mqueue sender and receiver example code

CC=gcc
CFLAGS=-Wall -g
LD=gcc
LDFLAGS=-g -lrt


OBJS1=producer.o
OBJS2=consumer.o

all: producer consumer

producer: $(OBJS1) 
	$(LD) $(LDFLAGS) -o $@ $(OBJS1) 

producer.o: producer.c 
	$(CC) $(CFLAGS) -c producer.c 

consumer: $(OBJS2) 
	$(LD) $(LDFLAGS) -o $@ $(OBJS2) 

consumer.o: consumer.c 
	$(CC) $(CFLAGS) -c consumer.c 



.PHONY: clean
clean:
	rm -f *.o *.out
	rm producer
	rm consumer
