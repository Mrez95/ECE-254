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
    // process id assigned to producer
    int pid = atoi(argv[2]);
    // number of producers in total
    int P = atoi(argv[3]);
    
    // From lab 4 - opens queue and creates messages
    // Queue already created in processes_main.c
    mqd_t qdes = mq_open(qname, O_RDWR); 
    
    // check if queue was opened successfully
    if (qdes == -1 ) {
        perror("mq_open()");
        exit(1);
    }

    // loop to generate N producers
    int i;
    for (i = 0; i < N; i++) {
        // Requirement: message / P = pid
        int message = (i * P) + pid;

        // check if mq_send() succeeded
        if (mq_send(qdes, (char*) &message, sizeof(int), 0) == -1) {
            perror("mq_send() failed");
        }
    }
    
    // *****************************************
    // final checks and cleaning up 
    // *****************************************
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }

    return 0;

}
