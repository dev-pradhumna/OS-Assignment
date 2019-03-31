#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ipc.h>
#include <semaphore.h>

#define N 25
time_t replace_time;
sem_t mutex,
      customers,
      barbers,
      prnt;
int count = 0;

void *barber(void *arg)
{
    while(time(NULL) < replace_time || count > 0)
    {
        sem_wait(&customers);
        sem_wait(&mutex);
        count--;        
        sem_wait(&prnt);
        printf("Barber is cutting hair\n");
        sem_post(&prnt);
        sem_post(&mutex);
        sem_post(&barbers);
        sleep(10);
    }
}

void *customer(void *arg)
{
    while(time(NULL) < replace_time)
    {
        sem_wait(&mutex);
        if(count < N)
        {
            count++;
            sem_wait(&prnt);
            printf("Customer arrived!\n");
            sem_post(&prnt);
            sem_post(&mutex);
            sem_post(&customers);
            sem_wait(&barbers);
        }
        else
            sem_post(&mutex);
        sleep(5);
    }
}

int main()
{
    pthread_t id1,
              id2;
    int status = 0;

    replace_time = time(NULL) + 30;

    sem_init(&mutex, 0, 1);
    sem_init(&customers,0 , 0);
    sem_init(&barbers, 0, 1);
    sem_init(&prnt, 0, 1);

    pthread_create(&id1, NULL, barber, NULL);
    // Customers initialization
    pthread_create(&id2, NULL, customer, NULL);
    // Joining
    pthread_join(id2, NULL);
    pthread_join(id1, NULL);

    exit(0);
}
