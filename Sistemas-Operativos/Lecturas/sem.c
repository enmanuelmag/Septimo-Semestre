#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct Lightswitch
{
    int counter;
    sem_t mutex;

    int (*lock)();
    int (*unlock)();
    int (*hello)();

} Lightswitch;

typedef struct arg_t
{
    Lightswitch *readS;
    Lightswitch *writeW;
    sem_t noReaders;
    sem_t noWriters;
} arg_t;

sem_t noReaders;
sem_t noWriters;
sem_t mutex;
Lightswitch readS;
Lightswitch writeS;
int validation = 0;

int lock(Lightswitch *self, sem_t semaphore)
{

    sem_wait(&mutex);

    if (++self->counter == 1)
    {
        sem_wait(&semaphore);
        
    }
    sem_post(&mutex);
}

int unlock(Lightswitch *self, sem_t semaphore)
{

    sem_wait(&mutex);

    if (--self->counter == 0)
    {
        sem_post(&semaphore);
    }
    sem_post(&mutex);
}

int hello()
{
    printf("LS created!!\n");
}

void init_lightswitch(Lightswitch *ls)
{

    ls->counter = 0;
    ls->lock = lock;
    sem_init(&mutex, 0, 1);
    ls->lock = lock;
    ls->unlock = unlock;
    ls->hello = hello;
}

void *read_r(void *args)
{

    arg_t *data = args;
    int i = 0;
    
    while (i < 9000)
    {
        sem_wait(&noReaders);
        
        
        readS.lock(&noWriters);
        sem_post(&noReaders);
        ++validation;
        
        //printf("Ta' buena la lectura my rey\n");

        readS.unlock(&noWriters);
        i++;
    }
}

void *write_r(void *args)
{

    arg_t *data = args;
    int i = 0;
    
    while (i < 2)
    {
        writeS.lock(&noReaders);
        
        sem_wait(&data->noWriters);
        printf("Escribiendo!\n");
        
        sem_post(&data->noWriters);
        writeS.unlock(&noReaders);
        i++;
    }
}

int main()
{

    
    init_lightswitch(&readS);

    
    init_lightswitch(&writeS);

    sem_init(&noReaders, 0, 1);
    sem_init(&noWriters, 0, 1);

    pthread_t *list_t = (pthread_t *)calloc(4, sizeof(pthread_t));
    arg_t *arg = calloc(1, sizeof(arg_t));
    arg->readS = &readS;
    arg->writeW = &writeS;
    arg->noReaders = noReaders;
    arg->noWriters = noWriters;

    for (int i = 0; i < 4; i++)
    {
        if (i < 3)
        {
            pthread_create(&list_t[i], NULL, read_r, arg);
        }
        else
        {
            pthread_create(&list_t[i], NULL, write_r, arg);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(list_t[i], NULL);
    }

    printf("Termine!");
    printf("%d", validation);

    return 0;
}