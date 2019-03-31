#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5

sem_t mutex;
sem_t wrtb;

int data = 0;
int rcount = 0;

void *reader(arg)
	void *arg;
{
	sem_wait(&mutex);

	++rcount;

	if (rcount == 1) {
		sem_wait(&wrtb);
	}

	sem_post(&mutex);
	printf("Data read by the reader %d is %d\n", (int)(__intptr_t) arg, data);
	sleep(1);
	sem_wait(&mutex);

	--rcount;

	if (rcount == 0) {
		sem_post(&wrtb);
	}

	sem_post(&mutex);
}

void *writer(arg)
	void *arg;
{
	sem_wait(&wrtb);

	++data;

	printf("Data written by the writer %d is %d\n", (int)(__intptr_t) arg, data);
	sleep(1);
	sem_post(&wrtb);
}

int main(void) {
	int b;
	pthread_t rtid[N], wtid[N];

	sem_init(&mutex, 0, 1);
	sem_init(&wrtb, 0, 1);

	for (int i = 0; i < N; ++i) {
		pthread_create(&wtid[i], NULL, writer, (void *)(__intptr_t) i);
		pthread_create(&rtid[i], NULL, reader, (void *)(__intptr_t) i);
	}
	for (int i = 0; i < N; ++i) {
		pthread_join(wtid[i], NULL);
		pthread_join(rtid[i], NULL);
	}
}
