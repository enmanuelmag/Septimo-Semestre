#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

#define NUM 5
#define SHMSZ 4
#define SHM_ADDR 233
#define NUM_THREADS 3

/*
 * STRUCTURES
 */
typedef struct arg_gyros
{
    int *on_gyro;
    int *men_gyro;
    sem_t sem_gyro;
} arg_gyros;

/*
 * GLOBAL VARIABLES 
 */
int EVENT_1 = 0;
int EVENT_3 = 0;
int EVENT_4 = 0;

int is_on = 0;
int landscape = 0;

int is_gyros1;
int is_gyros2;
sem_t sem_event_1;
sem_t sem_g1;
sem_t sem_g2;
sem_t sem_gas;
sem_t sem_princ_eng;

int shmid[NUM];
pthread_t array_threads[NUM_THREADS];
int *param[NUM], *distance, *gasoline, *gyros1, *gyros2, *alarm;

/* 
 * SINGAL'S HANDLE DEFINITION
 */
void handle_cod_101();
void handle_cod_102();
void handle_cod_103();
void handle_event_1(int sig);
void handle_event_3(int sig);
void handle_event_4(int sig);

/*
 * USER'S FUNCTIONS DEFINITION
 */
void go_up_30m();
void go_up_explode();
int init_shared_memory(void);

/*
 * THREAD'S ROUTINES 
 */
void *gyroscope_thread();
void *pricipal_engine_thread();

/*
 * CODE 
 */
int main(int argc, char *argv[])
{

    while (is_on && !landscape)
    {
        if (EVENT_3)
        {
            go_up_30m();
        }
        if (EVENT_4)
        {
            go_up_explode();
        }
    }
}

void go_up_30m()
{
    if (*distance < 30)
    {
        *distance += 1;
    }
    else
    {
        EVENT_1 = 1;
        EVENT_3 = 1;
        sem_post(&sem_g1);
        sem_post(&sem_g2);
    }
}

void go_up_explode(){
    sem_wait(&sem_gas);
    if (*gasoline > 5){
        *distance += 1;
    }else{
        is_on = 0;
        //sem_post(&sem_princ_eng);
        EVENT_1 = 0;
        EVENT_3 = 0;
        EVENT_4 = 0;
        for (int i = 0; i < NUM_THREADS; i++)
        {
            pthread_kill(array_threads[i], NULL);
        }
    }
    sem_post(&sem_gas);
}

int init_shared_memory(void)
{
    int i;

    for (i = 0; i < NUM; i++)
    {
        if ((shmid[i] = shmget(SHM_ADDR + i, SHMSZ, IPC_CREAT | 0666)) < 0)
        {
            perror("shmget");
            return (-1);
        }
        if ((param[i] = shmat(shmid[i], NULL, 0)) == (int *)-1)
        {
            perror("shmat");
            return (-1);
        }
    }

    distance = param[0];
    gasoline = param[1];
    gyros1 = param[2];
    gyros2 = param[3];
    alarm = param[4];

    return (1);
}

void *gyroscope_thread(void *arg)
{
    arg_gyros *info = arg;

    sem_t sem_gyro = info->sem_gyro;
    int *dir_gyros = info->men_gyro;
    int *on_gyro = info->on_gyro;

    while (is_on)
    {
        sem_wait(&sem_event_1);
        sem_wait(&sem_gyro);
        if (*dir_gyros)
        {
            *on_gyro = 1;

            sem_wait(&sem_gas);
            *gasoline -= 1;
            sem_post(&sem_gas);

            if (*dir_gyros > 0)
            {
                *dir_gyros -= 0.5;
            }
            else
            {
                *dir_gyros += 0.5;
            }
        }
        sem_post(&sem_gyro);
        sem_post(&sem_event_1);
    }
}

void *pricipal_engine_thread(void *arg)
{
    while (is_on)
    {
        sem_wait(&sem_princ_eng);
        sem_wait(&sem_g1);
        sem_wait(&sem_g2);
        sem_wait(&sem_gas);
        if (is_gyros1 || is_gyros2)
        {
            *gasoline -= 0.5;
        }
        else
        {
            *gasoline -= 1;
        }
        sem_post(&sem_gas);
        sem_post(&sem_g2);
        sem_post(&sem_g1);
        sem_post(&sem_princ_eng);
    }
}
