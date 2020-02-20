#include<stdio.h>
#include<iostream>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
//#include "1505095.h"

using namespace std;

//semaphore to control sleep and wake up
sem_t empty;
sem_t full;
queue<int> cake;
queue<string> caketype;
queue<int> chocolatecake;
queue<int> vanillacake;
pthread_mutex_t lock;

int k = 1;
int j = 1;

const char* choiceOpt[2] = {"vanilla", "chocolate"};

void init_semaphore()
{
	sem_init(&empty,0,5);
	sem_init(&full,0,0);
	pthread_mutex_init(&lock,0);
}

void * ChefFunc(void * arg)
{
	printf("%s\n",(char*)arg);
	while(1){	
		int i;
		srand (time(NULL));
//		cout<<cake.size()<<endl;
//		cout<<cake.empty()<<endl;
		if(cake.empty()){
			for(i=1;i<=5;i++)
			{
				int RandChoice = rand() % 2;

				sem_wait(&empty);

				pthread_mutex_lock(&lock);
  				if(choiceOpt[RandChoice] == "vanilla"){
  					cout <<"choice is : "<<choiceOpt[RandChoice]<<endl;		
					sleep(1);	
					cake.push(i);
					caketype.push("vanilla");
					printf("chef y produced vanillacake item %d\n",i);
  				}

  				pthread_mutex_unlock(&lock);
	
				sem_post(&full);
				sem_wait(&empty);

				pthread_mutex_lock(&lock);
  				if(choiceOpt[RandChoice] == "chocolate"){
  					cout <<"choice is : "<<choiceOpt[RandChoice]<<endl;		
					sleep(1);	
					cake.push(i);
					caketype.push("chocolate");
					printf("chef x produced chocolatecake item %d\n",i);
  				}
  				pthread_mutex_unlock(&lock);
	
				sem_post(&full);
			}
		}
//		cout<<cake.empty()<<endl;
		pthread_mutex_lock(&lock);
		if(cake.size() == 5){
			printf("Chef x and chef y taking break \n");
		}
		pthread_mutex_unlock(&lock);
	}
}

void * DecoratorFunc(void * arg)
{
	printf("%s\n",(char*)arg);
	//for(i=1;i<=10;i++)
	while(1)
	{	
		int i;
		sem_wait(&full);
 		
		pthread_mutex_lock(&lock);
			
		//sleep(1);
		if(cake.empty()){
			printf("chef z taking break\n");
		}
		else{
			int item = cake.front();
			string type = caketype.front();
			cake.pop();
			caketype.pop();
			printf("chef z picked %s cake %d\n",type.c_str(),item);
			if(type == "vanilla"){
				printf("Its vanilla cake %d\n",item);
				if(vanillacake.size()<=5){
					vanillacake.push(k);
					printf("vanillacake %d is pushed for waiter2\n", k);
					k++;
				}

			}
			if(type == "chocolate"){
				printf("Its chocolate cake %d\n",item);
				if(chocolatecake.size()<=5){
					chocolatecake.push(j);
					printf("chocolatecake %d is pushed for waiter1\n", j);
					j++;
				}
			} 	
		}
		pthread_mutex_unlock(&lock);
		
		sem_post(&empty);
	}
//	cout<<"chocolatecake size :"<<chocolatecake.size()<<endl;
//	cout<<"vanillacake size :"<<vanillacake.size()<<endl;
}

void * Waiter1Func(void * arg)
{
	printf("%s\n",(char*)arg);
	while(1){

		sem_wait(&empty);
 		
		pthread_mutex_lock(&lock);
		if(chocolatecake.empty()){
			printf("waiter1 is taking rest\n");
		}
		//while(chocolatecake.size() != 0){
		else{
			int item = chocolatecake.front();
			chocolatecake.pop();
			printf("waiter1 take chocolatecake item : %d\n",item );
			//sleep(1);
		}
		pthread_mutex_unlock(&lock);
		
		sem_post(&full);
	}
}

void * Waiter2Func(void * arg)
{
	printf("%s\n",(char*)arg);
	while(1){

		sem_wait(&empty);
 		
		pthread_mutex_lock(&lock);

		if(vanillacake.empty()){
			printf("waiter2 is taking rest\n");
		}
			// if(chocolatecake.empty()){
			// 	printf("waiter1 is taking rest\n");
			// }
			//while(vanillacake.size() != 0){
		else{
			int item = vanillacake.front();
			vanillacake.pop();
			printf("waiter2 take vanillacake item : %d\n",item );
				//sleep(1);
		}
		pthread_mutex_unlock(&lock);
		
		sem_post(&full);
	}
}

int main(void)
{	
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	
	init_semaphore();
	
	char * message1 = "chef x and chef y are producing";
	char * message2 = "chef z is ready for decorating cakes";
	char * message3 = "waiter1 are waiting";
	char * message4 = "waiter2 are waiting";	
	
	pthread_create(&thread1,NULL,ChefFunc,(void*)message1 );
	pthread_create(&thread2,NULL,DecoratorFunc,(void*)message2 );
	pthread_create(&thread3,NULL,Waiter1Func,(void*)message3 );
	pthread_create(&thread4,NULL,Waiter2Func,(void*)message4 );
	
	//while(1);
	//pthread_join(thread4, NULL);
	pthread_exit(NULL);
	return 0;
}
