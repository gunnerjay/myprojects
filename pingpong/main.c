#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

int resource = 0;
int resourceCounter = 0;
_Bool dataAvailable = 0;

pthread_mutex_t rwMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_consumed = PTHREAD_COND_INITIALIZER;
pthread_attr_t attr;

void* readerThread(void* threadArg) {
  printf("reader thread %lu\n", pthread_self());
  pthread_cond_broadcast(&c_consumed);
  
  _Bool exitLoop = 0;
  while (!exitLoop) {
    pthread_mutex_lock(&rwMutex);
      //printf("reader thread waiting\n");
      while (!dataAvailable) {
        pthread_cond_wait(&c_available, &rwMutex);
      }
      //printf("reader thread exit waiting\n");

      printf("read resource value %d\n", resource);
      dataAvailable = 0;
      if (resource == 1000) {
        exitLoop = 1;;
      }
    pthread_mutex_unlock(&rwMutex);

    if (!exitLoop) {
      pthread_cond_signal(&c_consumed);
    }    
  }

  printf("exiting thread %lu\n", pthread_self());

  return 0;
}

void* writerThread(void* threadArg) {
  printf("writer thread %lu\n", pthread_self());

  _Bool exitLoop = 0;
  while (!exitLoop) {
    pthread_mutex_lock(&rwMutex);
      //printf("writer thread waiting\n");
      while (dataAvailable) {
        pthread_cond_wait(&c_consumed, &rwMutex);
      }
      //printf("writer thread exit waiting\n");
      ++resource;
      printf("updated resource value %d\n", resource);
      dataAvailable = 1;
      if (resource == 1000) {
        exitLoop = 1;;
      }      
    pthread_mutex_unlock(&rwMutex);

    pthread_cond_signal(&c_available);
  }

  printf("exiting thread %lu\n", pthread_self());

  return 0;
}

int main() {
    srandom(time(NULL));
    int i;
    int numThreads = 2;

    pthread_t* pThreads = calloc(numThreads, sizeof(pthread_t));

    pthread_create(&pThreads[0], NULL, readerThread, NULL);
    sleep(1);
    pthread_create(&pThreads[0], NULL, writerThread, NULL);

    for (i = 0; i < numThreads; i++) {
        pthread_join(pThreads[i], NULL);
    }

    free(pThreads);
}