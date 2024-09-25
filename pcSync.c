#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
//variable and struct definitoin
#define BUFFER_SIZE 5
typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
int in = 0;
int out = 0;
sem_t full;
sem_t empty;
//functions definitions
void *producer(void *param);
void *consumer(void *param);
void *insert_item(buffer_item item);
void *remove_item();

int main(int argc, char* argv[]) {
    //declare varibles to store command line parameters
    int sleep_time;
    int producer_num;
    int consumer_num;
    // Check if any arguments were provided
    if (argc > 1) { 
        sleep_time = (int)atoi(argv[1]);
        producer_num = (int)atoi(argv[2]);
        consumer_num = (int)atoi(argv[3]);
    }
    else {
        printf("No command-line arguments were provided.\n");
    }

    //initialize semaphores full and empty
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
    //initialize mutex
    if(pthread_mutex_init(&mutex, NULL) != 0){
        printf("\n mutex init has failed\n");
        return 1;
    }
    //initialize empty buffer to -1
    for (int i = 0; i<BUFFER_SIZE; i++){
        buffer[i] = -1;
    }

    //create producer and consumer threads based on the number provided in the command line
    pthread_t tid_producer[producer_num];
    pthread_t tid_consumer[consumer_num];
    int error;
    //create producer threads
    for (int i = 0; i < producer_num; i++){
        //pass index i as unique thread index
        error = pthread_create(&tid_producer[i], NULL, producer, &i);
        if (error != 0){
            printf("\nThread can't be created: [%s]", strerror(error));
        }
    }
    //create consumer threads
    for (int i = 0; i<consumer_num;i++){
        error = pthread_create(&tid_consumer[i], NULL, consumer, &i);
        if (error != 0){
            printf("\nThread can't be created: [%s]", strerror(error));
        }
    }

    //sleep the main thread and then destroy semaphore and mutex variables and exit
    sleep(sleep_time);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);
    exit(0);
}
void *producer(void *param) {
    //get index parameter
    int index = *(int*)(param);
    buffer_item item;
    //random sleep duration from 1-4
    int sleep_duration = rand() % 4;
    while (true) {
        /* sleep for a random period of time: 0-4 seconds */
        sleep(sleep_duration);
        /* generate a random number */
        item = rand();
        
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        /* insert an item */
        printf("Producer %d produced item %d into buffer[%d]\n", index, item, in);
        insert_item(item);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer(void *param) {
    int index = *(int*)(param);
    int sleep_duration = rand() % 4;
    while (true) {
        /* sleep for a random period of time: 0-4 seconds */
        sleep(sleep_duration);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        /* remove an item */
        printf("Consumer %d consumed item %d from buffer[%d]\n", index, buffer[out], out);
        remove_item();
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}
void *insert_item(buffer_item item){
/* produce an item in next_produced */
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
}
void *remove_item(){
        buffer[out] = -1;
        /* consume the item in next consumed */
        out = (out + 1) % BUFFER_SIZE;
}