#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#define NUM_THREADS 6
#define MAXL 3

sem_t mutex;
sem_t turn;

sem_t empty;
sem_t noReaders2;

int noReaders = 0;
int leidas = 0;

void *writer()
{   
    
    while (1)
    {
        sem_wait(&turn);
        sem_wait(&empty);

        printf("Escribiendo!\n");
        
        sem_post(&turn);
        
        sem_post(&empty);
    }
}

void *reader()
{
    
    while (1)
    {   
        sem_wait(&turn);
        sem_post(&turn);

        sem_wait(&mutex);
        ++noReaders;
        
        if (noReaders == 1)
        {
            sem_wait(&empty);
        }
        sem_post(&mutex);

        printf("Leyendo!\n");
        

        sem_wait(&mutex);
        
        --noReaders;
        if (noReaders == 0)
        {
            sem_post(&empty);
        }
        sem_post(&mutex);
    }
}

int main()
{

    sem_init(&mutex, 0, 1);
    sem_init(&turn, 0, 1);
    
    sem_init(&empty, 0, 1);

    pthread_t *list_t = (pthread_t *)calloc(NUM_THREADS, sizeof(pthread_t));

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (i < 5)
        {
            pthread_create(&list_t[i], NULL, reader, NULL);
        }
        else
        {
            pthread_create(&list_t[i], NULL, writer, NULL);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(list_t[i], NULL);
    }

    printf("Termine!\n");

    return EXIT_SUCCESS;
}