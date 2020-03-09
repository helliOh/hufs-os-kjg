#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "LinkedListWithPthread.h"

dblList buf;//dblList is the double linked list minimized for this process
int lineCnt = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

void *producer(void *arg);
void *consumer(void *arg);

int main(){
  buf.init = initList;//init the buffer
  buf.init(&buf);
  pthread_t threads[2];
  pthread_create(&threads[0], NULL, producer, NULL);
  pthread_create(&threads[1], NULL, consumer, NULL);
  for(int i=0; i<2; i++) pthread_join(threads[i], NULL);

  return 0;
}
//List :: freeSpace() - returns freeSpace in the list which is 100 - current node amount
void *producer(void *arg){//insert integers into list
  int data;
  for(int i=0; i<1000; i++){
    pthread_mutex_lock(&mutex);
    if(buf.freeSpace(&buf) == 0)
      pthread_cond_wait(&buffer_has_space, &mutex);

    data = buf.insert(&buf, i);
    pthread_cond_signal(&buffer_has_data);
    pthread_mutex_unlock(&mutex);
  }
}

void *consumer(void *arg){//consume intergers into list and print it
  int data;
  for(int i=0; i<1000; i++){
    pthread_mutex_lock(&mutex);
    if(buf.freeSpace(&buf) == 100)
      pthread_cond_wait(&buffer_has_data, &mutex);

    data = buf.remove(&buf);
    pthread_cond_signal(&buffer_has_space);
    pthread_mutex_unlock(&mutex);
    printf("(%d)data =  %d\n", lineCnt++, data);
  }
}
