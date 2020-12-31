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

pthread_t tid;

void sig_handlerINT(int signo)
{
  int i;
  if (signo == SIGCONT)
  {
    printf("Desperté wey!\n");
  }
  if (signo == SIGTSTP)
  {
      printf("ME PAUSE\n");
      pause();
  }
  
  //pthread_cancel(tid);
  //exit(1);
  return;
}

void sig_handlerSTP(int signo)
{
  int i;
  if (signo == SIGTSTP)
  {
      printf("ME PAUSE\n");
      pause();
  }
  //pthread_cancel(tid);
  //exit(1);
  return;
}

void *thread(){

    if (signal(SIGCONT, sig_handlerINT) == SIG_ERR)
        printf("\nCan't catch SIGINT\n");
    if (signal(SIGTSTP, sig_handlerINT) == SIG_ERR)
        printf("\nCan't catch SIGINT\n");

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
        if (i == 12)
        {
            if (pthread_kill(tid, SIGTSTP) == 0)
            {
                printf("Lo pause\n");
            }else
            {
                printf("Error al pausar\n");
            }
        }
        
    }
    pthread_join(tid, NULL);
}