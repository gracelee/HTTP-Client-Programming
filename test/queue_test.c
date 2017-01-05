
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include "queue.h"

#define NUM_THREADS 16
#define N 1000000

typedef struct {
  int value; 
} Task;


void *doSum(void *arg) {
  int sum = 0;
  Queue *queue = (Queue*)arg;
  
  Task *task = (Task*)queue_get(queue);
  while(task) {
    sum += task->value;
    free(task);
    
    task = (Task*)queue_get(queue);
  } 
  
  pthread_exit((void*)(intptr_t)sum);
}



int main(int argc, char **argv) {

	clock_t begin = clock();
 
  int i, sum;

  pthread_t thread[NUM_THREADS];
  Queue *queue = queue_alloc(NUM_THREADS);

  
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_create( &thread[i], NULL, doSum, queue);
  }

  int expected = 0;
  for(i = 0; i < N; ++i) {
    Task *task = (Task*)malloc(sizeof(Task));
    task->value = i;
    queue_put(queue, task);
    expected += i;
  }
  

  for (i = 0; i < NUM_THREADS; ++i) {
    queue_put(queue, NULL);  
  }
  
  intptr_t value;
  sum = 0;
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_join(thread[i], (void**)&value);
    sum += value;
  }
  queue_free(queue);
  printf("total sum: %d, expected sum: %d\n", (int)sum, expected);
  clock_t end = clock();
  double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;
  printf("time taken: %f\n",time_spent);
  return 0;
}
