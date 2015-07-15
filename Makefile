# ECE254 lab5 Linux Port
# Makefile: build process and threading code

cc==gcc
CFLAGS=-Wall -o
LD=gcc
LDFLAGS=-g

process:
	gcc -o producer producer.c -lrt -lpthread
	gcc -o consumer consumer.c -lrt -lpthread -lm
	gcc -o produce processes_main.c -lrt -lpthread


.PHONY: clean
clean:
	rm -f *.o *.out
	rm producer
	rm consumer
	rm produce