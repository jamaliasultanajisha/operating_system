#include "queue.h"



student_node* new_student_node(int roll){
    student_node* t = NEW_STUDENT_NODE;
    t->stdId = roll;
    t->valid = 0;
    t->nxt = 0;
    sem_init(&t->sem, 0, 0);
    printf("Creating a student: %d\n\n", roll);
    return t;
}

queue* new_queue(int max_queue_size){
    queue* q = (queue*)malloc(sizeof(queue));

    q->max_len = max_queue_size;
    q->len = 0;
    q->head = 0;
    q->tail = 0;

    sem_init(&q->in_stock, 0, 0);
    sem_init(&q->free_space, 0, q->max_len);

    pthread_mutex_init(&q->lock, NULL);
    return q;
}

int enqueue( queue* q,  student_node* data){
    //printf("~~~~~~~~~~~~~~~~enqueing\n");
    sem_wait(&q->free_space);
    pthread_mutex_lock(&q->lock);

    if(q->head == 0){
        q->head = q->tail = data;
    }
    else{
        q->tail->nxt = data;
        q->tail = data;
    }
    q->len ++;

    pthread_mutex_unlock(&q->lock);
    sem_post(&q->in_stock);

    //printf("~~~~~~~~~~~~~~~~enqueing success\n");
    return SUCCESS;
}


student_node* dequeue(queue* q){
    sem_wait(&q->in_stock);
    pthread_mutex_lock(&q->lock);

    //printf("~~~~~~~~~~~~~~~~denqueing\n");

    student_node* t = q->head;

    q->len--;
    if(q->head == q->tail) q->head = q->tail = 0;
    else q->head = q->head->nxt;

    pthread_mutex_unlock(&q->lock);
    sem_post(&q->free_space);
    //printf("~~~~~~~~~~~~~~~~denqueing SUCCESS\n");
    return t;
}

int count_requests(queue* q, int roll){ // for the operation of thread B
    pthread_mutex_lock(&q->lock);
    int count = 0;

    student_node* t = q->head;
    while (t){
        if(t->stdId == roll) count++;
        t = t->nxt;
    }

    pthread_mutex_unlock(&q->lock);

    return count;
}

void delete_requests(queue* q, int roll){ // for the operation of thread B
    pthread_mutex_lock(&q->lock);

    student_node* t = q->head;
    student_node* prev = 0;
    while (t){
        if(t->stdId == roll) {
            //delete
            if(prev == 0){ //first node
                q->head = q->head->nxt;
                free(t);

                prev = 0;
                t = q->head;
            }
            else if(t->nxt == 0){ //last node
                prev->nxt = 0;
                q->tail = prev;

                break;
            }
            else{ //some internal node
                prev->nxt = t->nxt;
                free(t);

                t = prev->nxt;
            }

            if (q->head == 0){//empty queue
                q->tail = 0;
            }
        }
        else{
            prev = t;
            t = t->nxt;
        }
    }

    pthread_mutex_unlock(&q->lock);
}


void print_queue(queue* q){
    student_node* t = q->head;

    printf("\n::Queue:: \nMaxSize:%d\nCurrent size: %d\nElements: ", q->max_len, q->len);
    while (t){
        printf("%d, ", t->stdId);
        t = t->nxt;
    }
    printf("\n\n");
}
