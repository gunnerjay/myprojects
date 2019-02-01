#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "steque.h"

// This is a short program to test my understanding of pthreads, and the boss/worker pattern for multi-threading
// It uses a work queue to populate work items for the worker threads to pick up and process.  Requires synchronization
// objects to guard against MT issues.  I also experimented with main() being the boss thread, and then having
// a separate thread for the boss, which is why the commented out sections are in place.

struct context_t {
    int sockFd;
};

typedef struct context_t context_t;

steque_t gQueue;
const int TOTAL_REQUESTS = 100000;
int completedRequests = 0;
unsigned int totalRequests = 0;
pthread_mutex_t gGoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gDoWork = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWorkDone = PTHREAD_COND_INITIALIZER;
pthread_attr_t attr;

context_t* createContext() {
    context_t* data = calloc(1, sizeof(context_t));
    memset(data, 0, sizeof(context_t));

    return data;    
}

void* workerThread(void* threadArg) {
    int count = 0;
    int size = 0;
    _Bool exitThread = 0;
    _Bool broadcastDone = 0;
    while (!exitThread) {
        context_t* context;
        broadcastDone = 0;
        pthread_mutex_lock(&gGoMutex);
            while (totalRequests > 0 && steque_isempty(&gQueue)) {
                    printf("%lu waiting\n", pthread_self());
                    pthread_cond_wait(&gDoWork, &gGoMutex);
                    printf("%lu woke up\n", pthread_self());
                }

            if (totalRequests == 0) {
                exitThread = 1;
            } else {
                if (!steque_isempty(&gQueue)) {
                    context = (context_t*)steque_pop(&gQueue);
                    size = steque_size(&gQueue);
                    printf("thread %lu times: %d queue len %d context: %d \n", pthread_self(), ++count, size, context->sockFd);
                    totalRequests--; 
                    if (totalRequests == 0) broadcastDone = 1;
                    printf("completed request %d\n", totalRequests);
                }
            }
        pthread_mutex_unlock(&gGoMutex);
        if (broadcastDone) {
            printf("thred %lu brodcasting done\n", pthread_self());
            pthread_cond_broadcast(&gDoWork);
        }
    }

    printf("exiting thread %lu\n", pthread_self());

    return 0;
}

void* bossThread(void* threadArg) {
    int numThreads = *(int*)threadArg;

    totalRequests = TOTAL_REQUESTS;

    pthread_t* pThreads = calloc(numThreads, sizeof(pthread_t));

    int i;
    for (i = 0; i < numThreads; i++) {
        pthread_create(&pThreads[i], &attr, workerThread, NULL);
    }

    i = 0;
    while(i < TOTAL_REQUESTS) {
        context_t* context = createContext();
        context->sockFd = (random() % 6) + 1;
        pthread_mutex_lock(&gGoMutex);
            steque_push(&gQueue, context);
        pthread_mutex_unlock(&gGoMutex);

        pthread_cond_broadcast(&gDoWork);
        i++;
    }

    for (i = 0; i < numThreads; i++) {
        pthread_join(pThreads[i], NULL);
    }

    free(pThreads);

    return 0;
}

int main() {
    steque_init(&gQueue);

    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //pthread_attr_setschedpolicy(&attr, SCHED_RR);
    int workerCount = 5;
    pthread_t bossThreadId;
    pthread_create(&bossThreadId, &attr, bossThread, &workerCount);

    pthread_join(bossThreadId, NULL);
    
    //pthread_attr_destroy(&attr);
    steque_destroy(&gQueue);

    return 0;
}

// int main() {
//     steque_init(&gQueue);

//     int numThreads = 5;

//     pthread_t* pThreads = calloc(numThreads, sizeof(pthread_t));

//     int i;
//     for (i = 0; i < numThreads; i++) {
//         pthread_create(&pThreads[i], NULL, workerThread, NULL);
//     }

//     while(1) {
//         context_t* context = createContext();
//         context->sockFd = random() % 6;
//         pthread_mutex_lock(&gGoMutex);
//             steque_push(&gQueue, context);
//         pthread_mutex_unlock(&gGoMutex);

//         pthread_cond_signal(&gDoWork);
//         sleep(1);
//     }

//     free(pThreads);
//     steque_destroy(&gQueue);
// }