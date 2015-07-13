/*
 * processes_main.c
 * ECE254 Group 34
 *
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
#include <semaphore.h>

double get_time();

//constant queue_name for both producer and consumer.
const char* qname = "/mailbox_t94zhang";
const char* scname = "sem_consumer_t94zhang"; 

int main(int argc, char *argv[])
{
    // format should be ./produce <N> <B> <P> <C>
    // ie. early return for invalid input
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
    int C = atoi(argv[4]);
    
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
    
    // ********************************************
    // open up the queue
    // ********************************************
    qdes = mq_open(qname, O_RDWR | O_CREAT, mode,
                   &attr);
    
    // check if queue was opened successfully
    if (qdes == -1 ) {
        perror("mq_open()");
        exit(1);
    }

    // ********************************************
    // open up the semaphore
    // ********************************************
    sem_t *sem;
    sem = sem_open(scname, O_RDWR | O_CREAT,
            mode, N);
    
    // check if semaphore was opened successfully
    if (sem == SEM_FAILED ) {
        perror("sem_open()");
        exit(1);
    }
    
    // start tracking execution time
    double t_before_fork = get_time();

    pid_t pid_child;
    int p,c;

    // *******************************************
    // spawn producer processes 
    // *******************************************
    argv[0] = "producer";
    
    // loop to generate P producer processes 
    for (p = 0; p < P; p++){
        
        // properly cast int to char by pointers
        // note: in C int is recognized as char
        char* producer_id = (char*)&p;

        // assign unique producer id
        argv[2] = producer_id;
        
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
    
    
    // *******************************************
    // spawn consumer processes 
    // *******************************************
    
    argv[0] = "consumer";
    
    // loop to generate C consumer processes 
    for (c = 0; c < C; c++){
        
        // properly cast int to char by pointers
        // note: in C int is recognized as char
        char* consumer_id = (char*)&c;

        // assign unique consumer id
        argv[2] = consumer_id;

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
    
    // *****************************************
    // output timing results and data 
    // *****************************************
    
    // wait for all children to finish
    int status_child,pid;
    //wait(&status_child); 
    while ((pid = wait(&status_child)) != -1) {
    }
    // check if waiting was successful
    //if (WIFEXITED(status_child)) {
        // stop timer as execution is finished
        double t_last_consumed = get_time();
        // output results for analysis
        printf("System execution time: %f seconds\n",
               t_last_consumed - t_before_fork);
    // } else {
    //     perror("wait() failed");
    // }
    
    // *****************************************
    // final checks and cleaning up 
    // *****************************************

    // close queue
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }
    
    // remove queue
    if (mq_unlink(qname) == -1) {
        perror("mq_unlink() failed");
        exit(3);
    }

    if (sem_close(sem) == -1) {
        perror("sem_close() failed");
        exit(2);
    }

    if (sem_unlink(scname) == -1) {
        perror("sem_unlink() failed");
        exit(3);
    }

    return 0;
}

/* Helper function to get time in seconds */
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec / 1000000.0);
}
