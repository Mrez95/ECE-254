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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include <mqueue.h>
#include <math.h>

void * produce(void * id);
void * consume(void * id);
double get_time();
void printBuffer();

struct Buffer_node {
    struct Buffer_node * next;
    int val;
};

// Global vars
int N, B, P, C;
int count = 0; // size of linked list
int items_produced = 0;
int items_consumed = 0;
struct Buffer_node * head; // head of the linked list
struct Buffer_node * tail; // tail of the linked list
sem_t *sem_crit_region,
*sem_space_avail,
*sem_item_avail;

char * sem_crit_reg_name = "sem_crit";
char * sem_space_avail_name = "sem_space";
char * sem_item_avail_name = "sem_item";

int main(int argc, char * argv[]){
    double t_before;
    double t_after;
    mode_t mode = S_IRUSR | S_IWUSR;
    
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
    
    sem_crit_region = sem_open(sem_crit_reg_name, O_RDWR | O_CREAT, mode, 1);
    sem_space_avail = sem_open(sem_space_avail_name, O_RDWR | O_CREAT, mode, B);
    sem_item_avail = sem_open(sem_item_avail_name, O_RDWR | O_CREAT, mode, 0);
    
    if(sem_item_avail == SEM_FAILED || sem_space_avail == SEM_FAILED || sem_crit_region == SEM_FAILED){
        perror("sem_open");
    }
    
    pthread_t producers_threads[P];
    pthread_t consumers_threads[C];
    
    t_before = get_time();
    
    int p, c;
    // SPAWN PRODUCER THREADS ==> generates messages
    for (p = 0; p < P; p++) {
        int * producer_id = malloc(sizeof(int));
        *producer_id = p;
        //printf("Spawning producer with id %d \n\r", *producer_id);
        pthread_create(&producers_threads[p], NULL, produce, producer_id);
    }
    
    // SPAWN CONSUMER THREADS ==> consumes messages
    for (c = 0; c < C; c++) {
        int * consumer_id = malloc(sizeof(int));
        *consumer_id = c;
        //printf("Spawning consumer with id %d \n\r", *consumer_id);
        pthread_create(&consumers_threads[c], NULL, consume, consumer_id);
    }

    int p_j, c_j;
    // Join all producer threads
    for (p_j = 0; p_j < P; p_j++) {
        pthread_join(producers_threads[p_j], NULL);
    }
    
    // Join all consumer threads
    for (c_j = 0; c_j < C; c_j++) {
        pthread_join(consumers_threads[c_j], NULL);
    }
    
    
    t_after = get_time();
    
    printf("System execution time: %f seconds\n",
           t_after - t_before);
    
    if(sem_close(sem_crit_region) != 0 || sem_close(sem_space_avail) != 0 || sem_close(sem_item_avail) != 0){
        perror("sem_close");
    }
    if (sem_unlink(sem_crit_reg_name) != 0 || sem_unlink(sem_space_avail_name) != 0 || sem_unlink(sem_item_avail_name) != 0) {
	perror("sem_unlink");
    }
    return 0;
}

/* Helper function to get time in seconds */
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

void *produce(void *id){
    int producer_id = *((int *)id);
    int i;
    for (i = producer_id; i < N && items_produced < N; i += P) {
        
        int message = i;
        struct Buffer_node * new_node = malloc(sizeof(struct Buffer_node));
        new_node->next = NULL;
        new_node->val = message;
        
        sem_wait(sem_space_avail);
        
        sem_wait(sem_crit_region);
        //printf("Producer %d appending %d\n\r", producer_id, message);
        count++;
        // Append new message to tail
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
        items_produced++;
        //printf(" .... Items produced: %d\n\r ", items_produced);
        //printBuffer();
        sem_post(sem_crit_region);
        
        sem_post(sem_item_avail);
    }
    
    //printf("Producer %d exited\n\r", producer_id);
}

void *consume(void *id){
    int consumer_id = *((int *)id);
    int message;
    while (items_consumed < N) {
        sem_trywait(sem_item_avail);
        sem_wait(sem_crit_region);
	if(head != NULL){
            message = head->val; // take from the head
            //printf("Consumer %d consuming %d\n\r", consumer_id, message);
            if (count == 1) {
                free(head);
                head = NULL;
                tail = NULL;
            } else {
                struct Buffer_node * tmp = head;
                head = head->next;
                tmp = NULL;
            }
            count--;
            items_consumed++;
            // consume.
            if (sqrt(message) == (int)(sqrt(message))) {
                printf("%d %d %d\n\r", consumer_id, message, (int)(sqrt(message)));
            }
            //printBuffer();
	}
        sem_post(sem_crit_region);
        sem_post(sem_space_avail);
    }
    //printf("Consumer %d exited\n\r", consumer_id);
}

void printBuffer(){
    printf("\n **** ");
    struct Buffer_node *tmp = head;
    while (tmp != NULL) {
        printf("[%d] - ", tmp->val);
        tmp = tmp->next;
    }
    printf("[x], COUNT %d\n\r", count);
}

