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
#define NUM_GYROS 2
#define NUM_EVENTS 3
#define SHM_ADDR 233
#define NUM_THREADS 3

#define ABORT 40
#define DESBALANCE_A 42
#define DESBALANCE_B 44

/*
 * STRUCTURES
 */
typedef struct arg_gyros
{
    int id;
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

int main_id;
int is_on = 1;
int landscape = 0;

int is_gyros1;
int is_gyros2;

int state_gyros[NUM_GYROS];

sem_t sem_event_1;
sem_t sem_g1;
sem_t sem_g2;
sem_t sem_gs[NUM_GYROS];
sem_t sem_gas;
sem_t sem_princ_eng;

int shmid[NUM];
arg_gyros array_gyros[NUM_GYROS];
pthread_t array_threads[NUM_THREADS];
pthread_t array_threads_signals[NUM_THREADS];
int *param[NUM], *distance, *gasoline, *gyros1, *gyros2, *alarma;

/* 
 * SINGAL'S HANDLE DEFINITION
 */
void handle_cod_101();
void handle_cod_102();
void handle_cod_103();
//void handle_event_1(int sig);
void handle_event_3(int sig);
void handle_event_4(int sig);

/*
 * USER'S FUNCTIONS DEFINITION
 */
void go_up_30m();
void go_up_explode();
void throw_signals();
int init_shared_memory(void);

/*
 * THREAD'S ROUTINES 
 */
void *signal_gyro(void *arg);
void *signal_gyro_2();
void *pricipal_engine_thread();
void *gyroscope_thread(void *arg);

/*
 * CODE 
 */
int main(int argc, char *argv[])
{
    //Semaphore initialization
    for (int i = 0; i < NUM_GYROS; i++)
    {
        sem_init(&(sem_gs[i]), 0, 1);
    }
    //sem_init(&sem_g1, 0, 1);
    //sem_init(&sem_g2, 0, 1);
    sem_init(&sem_gas, 0, 1);
    sem_init(&sem_event_1, 0, 1);
    sem_init(&sem_princ_eng, 0, 1);

    if (init_shared_memory() == -1)
    {
        printf("Error en init memory\n");
    }

    printf("Iniciando...\n");
    //main_id = pthread_self();

    for (int i = 0; i < NUM_GYROS; i++)
    {
        arg_gyros *arg = &array_gyros[i];
        arg->id = i;
        arg->men_gyro = param[i + 2];
        arg->sem_gyro = sem_gs[i];
        state_gyros[i] = 0;
        arg->on_gyro = &(state_gyros[i]);

        int res;
        res = pthread_create(&array_threads_signals[i], NULL, signal_gyro, arg);
        if (res != 0)
        {
            perror("Creation of thread failed");
            exit(EXIT_FAILURE);
        }
    }

    pthread_create(&array_threads[2], NULL, pricipal_engine_thread, NULL);

    while (is_on && !landscape)
    {
        sleep(1);
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

void go_up_explode()
{
    sem_wait(&sem_gas);
    if (*gasoline > 5)
    {
        *distance += 1;
    }
    else
    {
        is_on = 0;
        //sem_post(&sem_princ_eng);
        EVENT_1 = 0;
        EVENT_3 = 0;
        EVENT_4 = 0;
        for (int i = 0; i < NUM_THREADS; i++)
        {
            //pthread_kill(array_threads[i], NULL);
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
    alarma = param[4];

    return (1);
}

void *gyroscope_thread(void *arg)
{
    arg_gyros *info = arg;
    printf("Nació hilo de correcniones giroscopio Nº %d\n", info->id);
    sem_t sem_gyro = info->sem_gyro;
    int *dir_gyros = info->men_gyro;
    int *on_gyro = info->on_gyro;
    while (is_on)
    {
        printf("Valor a corregir %d\n", *dir_gyros);
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
            sem_post(&sem_gyro);
            sem_post(&sem_event_1);
            sleep(1);
        }
        else
        {
            sem_post(&sem_gyro);
            sem_post(&sem_event_1);
            pthread_create(&array_threads_signals[0], NULL, signal_gyro, arg);
            break;
            //int res = pthread_kill(array_threads_signals[info->id], SIGCONT);
            //printf("Res: %d\n", res);
        }
    }
}

void *pricipal_engine_thread()
{
    while (is_on)
    {
        sleep(1);
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

void *signal_gyro(void *arg)
{

    arg_gyros *info = arg;
    printf("Nacio hilo del signal Nº %d\n", info->id);
    while (1)
    {
        sleep(1);
        printf("Analizando %d\n", info->id);
        if (*(info->men_gyro))
        {
            int res, err;
            res = pthread_create(&array_threads[0], NULL, gyroscope_thread, arg);
            if (res != 0)
            {
                perror("Creation of thread failed");
                exit(EXIT_FAILURE);
            }
            break;
            /* arg_gyros *arg = &array_gyros[0];
            arg->id = 1;
            arg->men_gyro = param[2];
            arg->on_gyro = &is_gyros1;
            arg->sem_gyro = sem_g1; 
            
            pthread_attr_t attr;
            res = pthread_attr_init(&attr);
            if (res != 0)
            {
                perror("Attribute init failed");
                exit(EXIT_FAILURE);
            }
            res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            if (res != 0)
            {
                perror("Setting detached state failed");
                exit(EXIT_FAILURE);
            }
            pthread_attr_destroy(&attr);
            pthread_kill(pthread_self(), SIGCONT);*/
        }
    }
}
