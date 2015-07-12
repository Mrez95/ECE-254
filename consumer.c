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

int main(int argc, char *argv[])
{	
	// format should be ./consumer <N> <B> <P> <C>
	if ( argc != 5 ) {
		exit(1);
	}

	mqd_t qdes; // queue_descriptor

    // unique id assigned for this consumer
    int id = atoi(argv[2]);

	// initialize a blocking queue
	attr.mq_maxmsg = B;
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

	int i = 0;
	// attempt to consume B times
	for (i = 0; i < N; i++){
		int msg; 
		int consumeMessage = mq_receive(qdes, (char*) &msg, sizeof(int),
				0);

		// error checking for message consume
		if (consumeMessage == -1) {
			perror("mq_receive() failed");
			exit(1);
		} 
		else {
			// printing here causes delay
			printf("%i is consumed\n", msg);
		}
	}

	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}

	return 0;
}
