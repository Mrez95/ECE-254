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
#include <pthread.h>
#include <semaphore.h>

struct buffer_node {
    struct buffer_node * next;
    int val;
};

// Global vars
int N, B, P, C, count;
buffer_node * head; // head of the linked list
sem_t sem_buffer; // Used to protect critical regions with regards to the buffer (e.g. modifying count etc.)

int main(int argc, char * argv[]){
    // format should be ./produce <N> <B> <P> <C>
    if (argc != 5) {
        exit(1);
    }
    
    // number of integers the producer should produce
    N = atoi(argv[1]);
    // number of integers the message queue can hold
    B = atoi(argv[2]);
    // number of producers
    P = atoi(argv[3]);
    // number of consumers
    c = atoi(argv[4]);
    
    // check for incorrect parameters
    if (N < 1 || B < 1 || P < 1 || C < 1){
        exit(1);
    }
    
    /* Initializing semaphores:
     * sem_init(sem_t *sem, int pshared, unsigned int value);
     * value = #threads allowed to be in crit. region
     * pshread: 0 = shared between threads of a process
     *         !0 = shared between processes
     * returns 0 on success
     */
    sem_init(&sem_buffer, 0, 1);
    
    return 0;
}