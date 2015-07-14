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

struct Buffer_node {
    struct Buffer_node * next;
    int val;
};

// Global vars
const int N, B, P, C;
int count = 0; // size of linked list
int items_processed = 0;
Buffer_node * head; // head of the linked list
Buffer_node * tail; // tail of the linked list
sem_t sem_crit_region,
        sem_space_avail,
        sem_item_avail;

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
    C = atoi(argv[4]);
    
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
    
    if(sem_init(&sem_crit_region, 0, 1) != 0 || sem_init(&sem_space_avail, 0, 1) != 0 || sem_init(&sem_item_avail, 0, 0) != 0){
        perror("sem_init");
    }
    
    pthread_t producers_threads[P];
    pthread_t consumers[C];
    
    double t_before = get_time();
    
    int p, c;
    // SPAWN PRODUCER THREADS ==> generates messages
    for (p = 0; p < P; p++) {
        argv[2] = p;
        pthread_create(producers[p], NULL, &produce, argv);
    }
    
    // SPAWN CONSUMER THREADS ==> consumes messages
    for (c = 0; c < P; c++) {
        argv[2] = c;
        pthread_create(consumers[p], NULL, &consume, argv);
    }
    
    
    
    int p_j, c_j;
    // Join all producer threads
    for (p_j = 0; p_j < P; p_j++) {
        pthread_join(producers[p_j], NULL);
    }
    
    // Join all consumer threads
    for (c_j = 0; c_j < C; c_j++) {
        pthread_join(consumers[c_j], NULL);
    }
    
    double t_after = get_time();
    
    printf("System execution time: %f seconds\n",
           t_after - t_before);
    
    if(sem_destroy(&sem_buffer) != 0){
        perror("sem_destroy");
    }
    return 0;
}

/* Helper function to get time in seconds */
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

void produce(void * args){
    printf("produce\r\n");
    int producer_id = atoi(args[2]);
    for (int i = 0; i < N / P; i++) {
        
        int message = producer_id + P * i;
        struct Buffer_node * new_node = malloc(sizeof(struct Buffer_node));
        new_node->next = NULL;
        new_node->val = message;
        
        sem_wait(sem_space_avail);
        
        sem_wait(sem_crit_region);
        
        count++;
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
        
        sem_post(sem_crit_region);
        
        sem_post(sem_item_avail);
    }
}

void consume(void * args){
    printf("consume\r\n");
    int message;
    int consumer_id = atoi(args[2]);
    while (items_processed < N) {
        sem_wait(sem_item_avail);
        sem_wait(sem_crit_region);
        
        message = head->val;
        if (count == 1) {
            head = NULL;
            tail = NULL;
        } else {
            struct * tmp = head;
            head = head->next;
            tmp = NULL;
        }
        count--;
        
        // consume.
        if (sqrt(message) == (int)(sqrt(message))) {
            printf("%d %d %d\n\r", )
        }
        
        sem_post(sem_crit_region);
        sem_post(sem_space_avail);
    }
}