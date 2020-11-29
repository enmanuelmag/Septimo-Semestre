#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 50

sem_t mutex;
sem_t filled;
sem_t empty;

void *producer()
{

    while (1)
    {
        sem_wait(&empty);
        sem_wait(&mutex);

        printf("Produciendo comidita c:!\n");

        sem_post(&filled);
        sem_post(&mutex);
    }
}

void *consumer()
{

    while (1)
    {
        sem_wait(&filled);
        sem_wait(&mutex);

        printf("Comiendo comidita c:!\n");

        sem_post(&empty);
        sem_post(&mutex);
    }
}

int main()
{

    sem_init(&mutex, 0, 1);
    sem_init(&filled, 0, 0);
    sem_init(&empty, 0, MAX);

    pthread_t *list_t = (pthread_t *)calloc(2, sizeof(pthread_t));

    for (int i = 0; i < 2; i++)
    {
        if (i < 1)
        {
            pthread_create(&list_t[i], NULL, producer, NULL);
        }
        else
        {
            pthread_create(&list_t[i], NULL, consumer, NULL);
        }
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_join(list_t[i], NULL);
    }

    printf("Termine!\n");

    return EXIT_SUCCESS;
}