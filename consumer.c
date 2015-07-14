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
#include <math.h>
#include <semaphore.h>

const char* qname = "/mailbox_t94zhang";
const char* scname = "sem_consumer_t94zhang"; 

int main(int argc, char *argv[])
{	
	// format should be ./produce <N> <B> <P> <C>
    // ie. early return for invalid input
	if ( argc != 5 ) {
		exit(1);
	}

	mqd_t qdes; // queue_descriptor
	mode_t mode = S_IRUSR | S_IWUSR; // permissions
	struct mq_attr attr; // queue attributes

    // unique id assigned for this consumer
    // argv[2] is a char pointer, take its value
    // in C int is recognized as char
    int id = *argv[2];

	// initialize a blocking queue
	attr.mq_maxmsg = id;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags = 0;

	// ********************************************
    // open up the queue
    // ********************************************
	qdes = mq_open(qname, O_RDWR | O_CREAT, mode,
                   &attr);

	// check if queue was opened successfully
	if (qdes == -1 ) {
		perror("mq_open() in consumer");
		exit(1);
	}

	// ********************************************
    // open up the semaphore
    // ********************************************

	sem_t *c_semaphore = sem_open(scname, 0);

	if (c_semaphore == SEM_FAILED) {
		perror("sem_open() in consumer");
		exit(1);
	}

	// ********************************************
    // wait for all items to be consumed
    // ********************************************

	int msg; // message to be received

	while(sem_trywait(c_semaphore) != -1) { // decrement & locks the semaphore

		// exit loop if and only if all callers stopped consuming
		// ie. semaphore counter reaches 0

		// returns the number of byes in the recieved msg. 
		// valid msg if return value > 0
		int isReceived = mq_receive(qdes, (char*) &msg, sizeof(int), 0);
		
		if (isReceived){
			// find perfect square
			if (((int)sqrt(msg) * (int)sqrt(msg)) == msg){
				printf("%i %i %i\n", id, msg, (int)sqrt(msg));
			}
		}
		else 
		{
			perror("mq_receive() in consumer");
			exit(1);
		}
	}

    // *****************************************
    // final checks and cleaning up 
    // *****************************************

	if (mq_close(qdes) == -1) {
		perror("mq_close() failed in consumer");
		exit(2);
	}

	if (sem_close(c_semaphore) == -1) {
		perror("sem_close failed in consumer");
		exit(2);
	}

	return 0;
}
