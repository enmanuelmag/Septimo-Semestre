#include <time.h>
#include <stdio.h>
#define __USE_GNU
#include <sched.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

void *thread(){

    int i = 0;
    while (1)
    {
        sleep(1);
        printf("Toy vivo!\n");
        ++i;
        if (i == 4)
        {
            pause();
        }
        
    }
    printf("Morí\n");
}

int main(){

    pthread_t tid;
    pthread_create(&tid, NULL, thread, NULL);
    printf("Ya creé el hilo\n");
    int i = 0;
    while (1)
    {
        sleep(1);
        ++i;
        if (i == 6)
        {
            if (pthread_kill(tid, SIGCONT) == 0)
            {
                printf("Lo desperté\n");
            }else
            {
                printf("Error al despertar\n");
            }
        }
    }
    pthread_join(tid, NULL);
}