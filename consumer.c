/*
 * consumer.c
 * ECE254 Group 34
 * By : Tianyi Zhang and Kk Yin Timothy Tong
 * University of Waterloo Computer Engineering
 * Fall 2015
 *
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <signal.h>
#include <semaphore.h>

const char* qname = "/mailbox_t94zhang";
const char* scname = "sem_consumer"; 

int main(int argc, char *argv[])
{	
	// format should be ./consumer <N> <B> <P> <C>
	if ( argc != 5 ) {
		exit(1);
	}

	mqd_t qdes; // queue_descriptor
	mode_t mode = S_IRUSR | S_IWUSR; // permissions
	struct mq_attr attr; // queue attributes

    // unique id assigned for this consumer
    int id = atoi(argv[2]);

	// initialize a blocking queue
	attr.mq_maxmsg = id;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags = 0;

	// open up the queue
	qdes = mq_open(qname, O_RDONLY, mode,
			&attr);

	// check if queue was opened successfully
	if (qdes == -1 ) {
		perror("mq_open()");
		exit(1);
	}

	// initialize and open up the semaphore
	sem_t *c_semaphore = sem_open(scname, 0);

	if (c_semaphore == SEM_FAILED) {
		perror("sem_open()");
		exit(1);
	}

	int msg; // message to be received

	// continously loop to consume until no more items to be expected
	for(;;) {

		// exit loop if and only if all callers stopped consuming
		// ie. semaphore counter reaches 0
		if (sem_trywait(c_semaphore) == -1) {
			break;
		}

		int isReceived = mq_receive(qdes, (char*) &msg, sizeof(int), 0);
		if (isReceived){
			// find perfect square
			if (((int)sqrt(msg) * (int)sqrt(msg)) == msg){
				printf("%i %i %i\n", id, msg, (int)sqrt(msg));
			}
		}
		else 
		{
			perror("mq_receive()");
			exit(1);
		}
	}

	// final closures and checks
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}

	if (sem_close(c_semaphore) == -1) {
		perror("sem_close failed in consumer");
		exit(2);
	}

	return 0;
}
