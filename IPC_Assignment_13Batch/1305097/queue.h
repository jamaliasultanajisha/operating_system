//
// Created by MUKTADIR on 3/30/2017.
//

#ifndef OFFLINES_QUEUE_H
#define OFFLINES_QUEUE_H

#include <stdio.h>
#include <malloc.h>
#include <semaphore.h>
#include <pthread.h>

#define FAIL -1
#define SUCCESS 1

#define INFINITY 9999999

#define NEW_STUDENT_NODE (student_node*)malloc(sizeof(student_node))

typedef struct StudentNode{ //the element of the queue
    int stdId;

    //if other thread wants to send data to the student thread, they will first validate "valid" variable, then buff.
    int valid;
    int buff;

    sem_t sem;

    //pointers to implement queue
    struct StudentNode* nxt;
} student_node;


typedef struct { //for each queue
    int max_len;
    int len; //current length of the queue
    student_node* head;
    student_node* tail;

    sem_t in_stock; //will be zero if there are no element
    sem_t free_space; //will be zero if there are "max_len" elements

    pthread_mutex_t lock;
} queue;

student_node* new_student_node(int roll);
queue* new_queue(int max_queue_size);
int enqueue( queue* q,  student_node* data);
student_node* dequeue(queue* q);
void print_queue(queue* q);

int count_requests(queue* q, int roll);
void delete_requests(queue* q, int roll);



#endif //OFFLINES_QUEUE_H
