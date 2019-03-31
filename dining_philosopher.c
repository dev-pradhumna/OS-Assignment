#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 

#define N 5

int states[N]; // Denoting what is the state of nth philosopher: 0 - Eating, 1 - Hungry, 2 - Thinking
int phil[N] = { 0, 1, 2, 3, 4 }; 

sem_t mutex; 
sem_t philosophers[N]; 

void check(int philosopher_index) { 
    if (states[philosopher_index] == 1 
        && states[(philosopher_index + 4) % N] != 0 
        && states[(philosopher_index + 1) % N ] != 0) { 
         
        states[philosopher_index] = 0; 

        sleep(2);
        printf("Philosopher %d is Eating\n", philosopher_index + 1); 
        sem_post(&philosophers[philosopher_index]); 
    } 
} 

void pickup_chopsticks(int philosopher_index) { 
    sem_wait(&mutex); 
    states[philosopher_index] = 1; 
    printf("Philosopher %d is Hungry\n", philosopher_index + 1); 
    check(philosopher_index); 
    sem_post(&mutex);
    sem_wait(&philosophers[philosopher_index]); 
    sleep(1); 
} 

void putdown_chopsticks(int philosopher_index){ 
    sem_wait(&mutex);
    states[philosopher_index] = 2; 
    printf("Philosopher %d is thinking\n", philosopher_index + 1); 
    check((philosopher_index + 4) % N);
    check((philosopher_index + 1) % N);
    sem_post(&mutex); 
} 

void* philospher(void* num){ 
    while (1) { 
        int* i = num; 
        sleep(1); 
        pickup_chopsticks(*i); 
        sleep(0); 
        putdown_chopsticks(*i); 
    } 
} 

int main() 
{ 
    int i; 
    pthread_t workers[N]; 

    // Semaphores initialization 
    sem_init(&mutex, 0, 1); 
    for (i = 0; i < N; i++) 
        sem_init(&philosophers[i], 0, 0); 
    // Create threads
    for (i = 0; i < N; i++) { 
        pthread_create(&workers[i], NULL, philospher, &phil[i]); 
        printf("Philosopher %d is thinking\n", i + 1); 
    } 
    // Join the threads
    for (i = 0; i < N; i++) 
        pthread_join(workers[i], NULL);
} 
