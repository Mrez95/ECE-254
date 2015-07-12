/*
 * processes_main.c
 * ECE254 Group 34
 * By: Tianyi Zhang and Kwok Yin Timothy Tong
 * University of Waterloo Computer Engineering
 * Fall 2015
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

double get_time();

//constant queue_name for both producer and consumer.
const char* qname = "/mailbox_t94zhang";

int main(int argc, char *argv[])
{
    // format should be ./processes_main <N> <B> <P> <C>
    if (argc != 5) {
        exit(1);
    }
    
    // number of integers the producer should produce
    int N = atoi(argv[1]);
    // number of integers the message queue can hold
    int B = atoi(argv[2]);
    // number of producers
    int P = atoi(argv[3]);
    // number of consumers
    int c = atoi(argv[4]);
    
    // check for incorrect parameters
    if (N < 1 || B < 1 || P < 1 || C < 1){
        exit(1);
    }
    
    mqd_t qdes;                         // queue_descriptor
    mode_t mode = S_IRUSR | S_IWUSR;    // permissions
    struct mq_attr attr;                // queue attributes
    
    // initialize a blocking queue
    attr.mq_maxmsg = B;
    attr.mq_msgsize = sizeof(int);
    attr.mq_flags = 0;
    
    // open up the queue
    qdes = mq_open(qname, O_RDWR | O_CREAT, mode,
                   &attr);
    
    // check if queue was opened successfully
    if (qdes == -1 ) {
        perror("mq_open()");
        exit(1);
    }
    
    // get time BEFORE program gets run
    double t_before_fork = get_time();
    int p, c;
    pid_t pid_child;  // Might want to move this into loop
    
    
    /******************************
     ** SPAWN PRODUCER PROCESSES **
     ******************************/
    argv[0] = "producer";
    
    // loop that spawns P producer processes
    for (p = 0; p < P; p++){
        
        argv[2] = (char)p; // unique identity number
        
        pid_child = fork();
        
        // check if forking was successful
        if (pid_child < 0){
            perror("fork()");
            exit(1);
        } else if (pid_child == 0) {
            // produce and send items
            execvp("./producer", argv);
        }
    }
    
    
    /******************************
     ** SPAWN CONSUMER PROCESSES **
     ******************************/
    
    argv[0] = "consumer";
    
    // loop that spawns C consumer processes
    for (c = 0; c < C; c++){
        
        argv[2] = (char)c; // unique identity number
        
        pid_child = fork();
        // check if forking was successful
        if (pid_child < 0){
            perror("fork()");
            exit(1);
        } else if (pid_child == 0) {
            // receive and consume items
            execvp("./consumer", argv);
        }
    }
    
    
    /***************************************
     ** OBTAINING RESULTS AND CLEANING UP **
     ***************************************/
    
    // wait for consumer
    int status_child;
    wait(&status_child); // maybe need busy loop?
    
    // check if waiting was successful
    if (WIFEXITED(status_child)) {
        double t_last_consumed = get_time();
        
        printf("System execution time: %f seconds\n",
               t_last_consumed - t_before_fork);
    } else {
        perror("wait() failed");
    }
    
    // close queue
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }
    
    // remove queue
    if (mq_unlink(qname) != 0) {
        perror("mq_unlink() failed");
        exit(3);
    }
    
    // TODO: sem_close, sem_unlink
    return 0;
}


/* Helper function to get time in seconds */
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec / 1000000.0);
}
