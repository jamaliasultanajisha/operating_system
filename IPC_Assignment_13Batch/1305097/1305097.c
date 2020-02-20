#include <semaphore.h>
#include <pthread.h>
#include "queue.h"

#include <time.h>

#define N_STUDENT_SIMULATED 30
#define STUDENT_ID_RANGE 5

queue* q_applications;
queue* q_req_sheets;
queue* q_pass_reqs;
queue* approved_reqs;
queue* generated_pass;

sem_t b; //If the request has not yet approved by A, C, E, then B must sleep.

int got_pass [N_STUDENT_SIMULATED];//for debugging purpose.

void initialize(){
    q_applications = new_queue(10);
    q_req_sheets = new_queue(1);
    q_pass_reqs = new_queue(4);
    approved_reqs = new_queue(INFINITY);
    generated_pass = new_queue(INFINITY);

    sem_init(&b, 0, 0);

    //for debugging
    memset(got_pass, 0, sizeof(got_pass));
}

void * student(void* a){
    int roll = *(int*) a;
    printf("%d: new student: %d\n\n", roll, roll);
    student_node* t = new_student_node(roll);


    enqueue(q_applications, t);
    printf("%d: Submitted application.\n\n", roll);
    sleep( 2 );
    enqueue(q_req_sheets, t);
    printf("%d: Submitted request sheet.\n\n", roll);
    sleep( 2 );

    int got_password = 0;
    while (!got_password){
        enqueue(q_pass_reqs, t);//periodically
        printf("%d: Submitted password request.\n\n", roll);
        sem_init(&t->sem, 0, 0);
        sem_wait(&t->sem);

        if(t->valid){
                printf("~~~~~~~~~~~~~~~~~~~~%d got pass: %d\n\n", roll, t->buff);
                break;
        }
        printf("%d: password did not reached.\n\n", roll);

        //sleep for some time;
        sleep( 2 );

    }
    printf("%d : completed :) \n\n", roll);
/*
    got_pass[roll] = 1;
    int i ;
    for(i = 0 ; i < N_STUDENT_SIMULATED; i ++) printf("%d:%d\n", i, got_pass[i]);
*/
    free(t);
}

void * application_approver(void* a){ //A, C, E

    char c = *((char*) a);
    while (1){
        printf("%c : Activated\n\n", c);
        student_node *app = dequeue(q_applications);

        enqueue(approved_reqs, new_student_node(app->stdId)); //created a duplicate copy in case student dies.
        printf("%c: approved %d\n\n", c, app->stdId);
        sem_post(&b);//wakeup b if waiting
    }
}


void* duplicate_checker(void* a){ // B

    while (1){
        printf("B : Activated\n\n");
        int roll = dequeue(q_req_sheets)->stdId;
        printf("B: got request: %d\n\n", roll);

        int i = count_requests(approved_reqs, roll);

        while (i == 0){ // no request yet. so wait.
            //wait for another approval by A/C/E to arrive.
            sem_init(&b, 0, 0);
            sem_wait(&b);

            i = count_requests(approved_reqs, roll);
        }

        if(i > 1){//duplicate copies, so delete all the requests
            printf("B: %d has %d requests. So ignoring\n\n", roll, i );
        }
        else{//not duplicate. so approve the request and send to D
            delete_requests(approved_reqs, roll);
            enqueue(generated_pass, new_student_node(roll));
            printf("B: approved %d\n\n", roll);
        }
    }

}

void* password_provider(void *a){ //D
    while(1){
        printf("D : Activated\n\n");
        student_node* t = dequeue(q_pass_reqs);
        printf("D: received password request : %d \n\n", t->stdId);

        int c = count_requests(generated_pass, t->stdId);

        if(c > 0){ //passed all the approvals, so give it a password
            srand(time(NULL));   // should only be called once
            t->buff =  12345678;//rand() % STUDENT_ID_RANGE;
            t->valid = 1;
            delete_requests(generated_pass, t->stdId);
            printf("D : sent password (%d ) to %d\n\n", t->buff, t->stdId);

        }
        printf("D : %d has %d passwords\n\n", t->stdId, c);
        sem_post(&t->sem);
    }
}


int main(){
    //freopen("out.txt", "w", stdout);
    initialize();

    int i, j, k;

    //student thread creation
    pthread_t t[N_STUDENT_SIMULATED];
    int roll[N_STUDENT_SIMULATED];
    for (i = 0; i < N_STUDENT_SIMULATED; i++){
        srand(time(NULL));   // should only be called once
        roll[i] = i;//rand() % STUDENT_ID_RANGE;
        printf("Generated: %d\n\n", roll[i]);
        pthread_create(&t[i], NULL, student, (void*) &roll[i]);
    }

    //A, C, E
    pthread_t ta, tb, tc, td, te;
    char a[] ={'A', 'C', 'E'};
    pthread_create(&ta, NULL, application_approver, (void*)&a[0]);
    pthread_create(&tc, NULL, application_approver, (void*)&a[1]);
    pthread_create(&te, NULL, application_approver, (void*)&a[2]);

    pthread_create(&tb, NULL, duplicate_checker, NULL); //B
    pthread_create(&td, NULL, password_provider, NULL); //D

    pthread_join(td, NULL);

    pthread_exit(NULL);
}
