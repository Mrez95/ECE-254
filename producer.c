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
    if (argc != 5) {
        exit(1);
    }

    /*
    pid = process id assigned to producer
    P = number of producers in total
    N = number of numbers produced in a set
    */

    int N = atoi(argv[1]);
    int pid = atoi(argv[2]);
    int P = atoi(argv[3]);
    
    // From lab 4 - opens queue and creates messages
    mqd_t qdes = mq_open(qname, O_RDWR); // Queue already created in processes_main.c
    
    // check if queue was opened successfully
    if (qdes == -1 ) {
        perror("mq_open()");
        exit(1);
    }

    int i;
    for (i = 0; i < N; i++) {
        int message = (i * P) + pid; // Requirement: message / P = pid
        if (mq_send(qdes, (char*) &message, sizeof(int), 0) == -1) {
            perror("mq_send() failed");
        }
    }
    
    // Close the queue
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }

    return 0;

}
