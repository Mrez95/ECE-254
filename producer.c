/*
 * processes_main.c
 * ECE254 Group 34
 * By: Tianyi Zhang and Kwok Yin Timothy Tong
 * University of Waterloo Computer Engineering
 * Fall 2015
 */

#include <stdio.h>
const char* qname = "/mailbox_t94zhang";

int pid, P, N;
/*
 pid = process id assigned to producer
 P = number of producers in total
 N = number of numbers produced in a set
 */

int main(int argc,  char *argv[]){
    if (argc != 5) {
        exit(1);
    }
    pid = atoi(argv[1]);
    P = 
}