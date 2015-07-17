# ECE254 lab5 Linux Port
# Makefile: build process and threading code

# BUILD INSTRUCTIONS: 

# type ‘make process’ in the terminal to build the multi-process implementation
# type ‘make threads’ in the terminal to build the multi-threading implementation
# type ‘make clean’ in the terminal to clean the output files

cc==gcc
CFLAGS=-Wall -o
LD=gcc
LDFLAGS=-g

process:
	gcc -o producer producer.c -lrt -lpthread
	gcc -o consumer consumer.c -lrt -lpthread -lm
	gcc -o produce processes_main.c -lrt -lpthread

threads:
	gcc -o produce threads_main.c -lrt -lm -lpthread

.PHONY: clean
clean:
	rm -f *.o *.out
	rm producer
	rm consumer
	rm produce
