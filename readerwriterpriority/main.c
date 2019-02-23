#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

int resource = 0;
int resourceCounter = 0;
int writeWaiters = 0;


pthread_mutex_t rwMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t readPhase = PTHREAD_COND_INITIALIZER;
pthread_cond_t writePhase = PTHREAD_COND_INITIALIZER;
pthread_attr_t attr;


void* readerThread(void* threadArg) {
  printf("reader thread %lu\n", pthread_self());
  pthread_mutex_lock(&rwMutex);
    if (writeWaiters > 0) {
      printf("kicking in writer priority %d\n", writeWaiters);
      pthread_cond_signal(&writePhase);
      pthread_cond_wait(&readPhase, &rwMutex);
    }

    while (resourceCounter == -1) {
      pthread_cond_wait(&readPhase, &rwMutex);
    }
    resourceCounter++;
  pthread_mutex_unlock(&rwMutex);

  printf("read resource value %d\n", resource);

  pthread_mutex_lock(&rwMutex);
    resourceCounter--;
    if (resourceCounter == 0) {
      pthread_cond_signal(&writePhase);
    }
  pthread_mutex_unlock(&rwMutex);

  //printf("exiting thread %lu\n", pthread_self());

  return 0;
}

void* writerThread(void* threadArg) {
  printf("writer thread %lu\n", pthread_self());
  pthread_mutex_lock(&rwMutex);
    writeWaiters++;
    while (resourceCounter != 0) {
      pthread_cond_wait(&writePhase, &rwMutex);
    }
    writeWaiters--;
    resourceCounter = -1;
  pthread_mutex_unlock(&rwMutex);

  ++resource;
  printf("updated resource value %d\n", resource);

  pthread_mutex_lock(&rwMutex);
    resourceCounter = 0;
    pthread_cond_signal(&writePhase);
    pthread_cond_broadcast(&readPhase);
  pthread_mutex_unlock(&rwMutex);

  //printf("exiting thread %lu\n", pthread_self());

  return 0;
}

int main() {
    srandom(time(NULL));
    int numThreads = 25;

    pthread_t* pThreads = calloc(numThreads, sizeof(pthread_t));

    int i;
    for (i = 0; i < numThreads; i++) {
        if (random() % 2) {
          pthread_create(&pThreads[i], NULL, writerThread, NULL);
        } else {
          pthread_create(&pThreads[i], NULL, readerThread, NULL);
        }
    }

    for (i = 0; i < numThreads; i++) {
        pthread_join(pThreads[i], NULL);
    }

    free(pThreads);
}