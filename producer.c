/*
 * producer.c
 * ECE254 Group 34
 * By: Tianyi Zhang and Kwok Yin Timothy Tong
 * University of Waterloo Computer Engineering
 * Fall 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>

const char* qname = "/mailbox_t94zhang";

int main(int argc,  char *argv[]){

    // format should be ./produce <N> <B> <P> <C>
    // ie. early return for invalid input
    if (argc != 5) {
        exit(1);
    }

    // number of numbers produced in a set
    int N = atoi(argv[1]);
    // number of producers in total
    int P = atoi(argv[3]);
    // argv[2] is a char pointer, take its value
    int pid = *argv[2];
    
    // ********************************************
    // open up the queue
    // ********************************************

    mqd_t qdes = mq_open(qname, O_RDWR); 
    
    // check if queue was opened successfully
    if (qdes == -1 ) {
        perror("mq_open() failed in producer");
        exit(1);
    }

    // ********************************************
    // make producers which statisfy condition
    // ********************************************

    int msg = pid;
    for (msg; msg < N; msg+=P) {
        // Requirement: message / P = pid

        int sendMsg = mq_send(qdes, (char*) &msg, sizeof(int), 0);
        // check if mq_send() succeeded
        if (sendMsg == -1) {
            perror("mq_send() failed in producer");
        }
    }
    
    // *****************************************
    // final checks and cleaning up 
    // *****************************************

    if (mq_close(qdes) == -1) {
        perror("mq_close() failed in producer");
        exit(2);
    }

    return 0;
}
