
#include "queue.h"



#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)



typedef struct QueueStruct {
	sem_t read;
	sem_t write;
	pthread_mutex_t lock;
	int size;
	int nextin;
    int nextout;
	void** buffer;
} Queue;


Queue *queue_alloc(int size) {
	Queue *queue =(Queue *)malloc(sizeof(Queue));
	queue->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	sem_init(&queue->read, 0, 0);
	sem_init(&queue->write, 0, size);
	queue->nextin = 0;
	queue->nextout = 0;
	queue->buffer =  (void**)malloc(sizeof(void**)*size);
	queue->size = size;
	return queue;

}

void queue_free(Queue *queue) {
	free(queue->buffer);
	free(queue);
}

void queue_put(Queue *queue, void *item) {
	sem_wait(&queue->write);
	pthread_mutex_lock(&queue->lock);
	queue->buffer[(queue->nextin++)%(queue->size)] = item;
    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->read);
}

void *queue_get(Queue *queue) {
	void* item;
	sem_wait(&queue->read);
    pthread_mutex_lock(&queue->lock);
	item = queue->buffer[(queue->nextout++)%(queue->size)];
    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->write);
    return(item);
}
