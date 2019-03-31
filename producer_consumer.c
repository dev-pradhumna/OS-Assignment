#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
	use the lpthread flag with gcc to compile this code
	~$ gcc q1.c -lpthread
*/

pthread_t *producers;
pthread_t *consumers;

/*
	mutex: binary semaphore to access critical section
	empty: counting semaphore to indicate number of free slots in the buffer
	fill: counting semaphore to indicate number of occupied slots
*/

sem_t mutex,empty,fill;

int *buf,buf_pos=-1,number_of_producers,number_of_consumers,buf_len;

int produce(pthread_t self){
	int i = 0;
	int item_produced = 1 + rand()%40;
	while(!pthread_equal(*(producers+i),self) && i < number_of_producers){
		i++;
	}
	printf("Producer %d produced %d \n",i+1,item_produced);
	return item_produced;
}


void consume(int p,pthread_t self){
	int i = 0;
	while(!pthread_equal(*(consumers+i),self) && i < number_of_consumers){
		i++;
	}

	printf("Buffer:");
	for(i=0;i<=buf_pos;++i)
		printf("%d ",*(buf+i));
	printf("\nConsumer %d consumed %d \nCurrent buffer len: %d\n",i+1,p,buf_pos);
	
}


void* producer(void *args){

	while(1){
		int item_produced = produce(pthread_self());
		/*
			producer should wait for atleast one slot to be empty 
			in the buffer to produce an item
		*/
		sem_wait(&empty); 
		sem_wait(&mutex); //acquire the mutex to access critical section
		++buf_pos;			// critical section
		*(buf + buf_pos) = item_produced; 
		sem_post(&mutex);
		sem_post(&fill);
		sleep(1 + rand()%3);
	}
	
	return NULL;
}


void* consumer(void *args){
	int c;
	while(1){
		/*
			consumer should wait for atleast one slot to be full 
			in the buffer to consume an item
		*/
		sem_wait(&fill);
		sem_wait(&mutex); //acquire the mutex to access critical section
		c = *(buf+buf_pos);
		consume(c,pthread_self());
		--buf_pos;
		sem_post(&mutex);
		sem_post(&empty);
		sleep(1+rand()%5);
	}

	return NULL;
}

int main(void){
	
	srand(time(NULL));

	/*
		initialize the semaphores
	*/

	sem_init(&mutex,0,1);
	sem_init(&fill,0,0); //initially buffer is empty

	printf("Enter the number of Producers:");
	scanf("%d",&number_of_producers);
	producers = (pthread_t*) malloc(number_of_producers*sizeof(pthread_t));

	printf("Enter the number of Consumers:");
	scanf("%d",&number_of_consumers);
	consumers = (pthread_t*) malloc(number_of_consumers*sizeof(pthread_t));

	printf("Enter buffer capacity:");
	scanf("%d",&buf_len);
	buf = (int*) malloc(buf_len*sizeof(int));

	sem_init(&empty,0,buf_len);

	/*
		Create the producer and consumer threads
	*/

	for(int i=0;i<number_of_producers;i++){
		int err = pthread_create(producers+i,NULL,&producer,NULL);
		if(err != 0){
			printf("Error creating producer %d: %s\n",i+1,strerror(err));
		}
	}

	for(int i=0;i<number_of_consumers;i++){
		int err = pthread_create(consumers+i,NULL,&consumer,NULL);
		if(err != 0){
			printf("Error creating consumer %d: %s\n",i+1,strerror(err));
		}
	}

	/*
		Join the threads that we created
	*/
	for(int i=0;i<number_of_producers;i++){
		pthread_join(*(producers+i),NULL);
	}
	for(int i=0;i<number_of_consumers;i++){
		pthread_join(*(consumers+i),NULL);
	}


	return 0;
}
