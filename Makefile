cc==gcc
CFLAGS=-Wall -o
LD=gcc
LDFLAGS=-g

process:
	gcc -o producer producer.c -lrt -lpthread
	gcc -o consumer consumer.c -lrt -lpthread -lm
	gcc -o produce processes_main.c -lrt -lpthread