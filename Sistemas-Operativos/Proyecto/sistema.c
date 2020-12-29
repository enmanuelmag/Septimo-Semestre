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
#define NUM_THREADS 5
#define NUM_THREADS_SIGNALS 4
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
int INTERVAL = 1;

pthread_t main_id;
int is_on = 1;
int landscape = 0;
int state_gyros[NUM_GYROS];

sem_t sem_gas;
sem_t sem_gs[NUM_GYROS];

int shmid[NUM];
arg_gyros array_gyros[NUM_GYROS];
pthread_t array_threads[NUM_THREADS];
pthread_t array_threads_signals[NUM_THREADS_SIGNALS];
int *param[NUM], *distance, *gasoline, *gyros1, *gyros2, *alarma;

/* 
 * SINGAL'S HANDLE DEFINITION
 */
void handle_cod_101();
void handle_cod_102();
void handle_cod_103();

/*
 * USER'S FUNCTIONS DEFINITION
 */
void terminate_event_1();
void terminate_event_3();
void create_signals_gyros();
int init_shared_memory(void);

/*
 * THREAD'S ROUTINES 
 */
void *go_up_30m();
void *go_up_explode();
void *signal_gyro(void *arg);
void *signal_explote_rocket();
void *signal_restart_landing();
void *pricipal_engine_thread();
void *gyroscope_thread(void *arg);

/*
 * CODE 
 */
int main(int argc, char *argv[])
{
    for (int i = 0; i < NUM_GYROS; i++)
    {
        sem_init(&(sem_gs[i]), 0, 1);
    }
    sem_init(&sem_gas, 0, 1);

    main_id = pthread_self();
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    if (init_shared_memory() == -1)
    {
        printf("Error en init memory\n");
    }
    printf("Init...\n");
    create_signals_gyros();
    pthread_create(&array_threads[2], NULL, pricipal_engine_thread, NULL);

    pthread_create(&array_threads_signals[2], NULL, signal_restart_landing, NULL);
    pthread_create(&array_threads_signals[3], NULL, signal_explote_rocket, NULL);

    while (is_on && !landscape)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        printf("Valor actual distancia %d combustible %d giro1 giro2 alarma= %d %d %d \n",
               *distance, *gasoline, *gyros1, *gyros2, *alarma);
    }
}

void create_signals_gyros()
{
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

void terminate_event_1()
{
    pthread_cancel(array_threads[0]);
    pthread_cancel(array_threads[1]);
    pthread_cancel(array_threads_signals[0]);
    pthread_cancel(array_threads_signals[1]);
}

void terminate_event_3()
{
    pthread_cancel(array_threads[3]);
    pthread_cancel(array_threads_signals[2]);
}

/*
 * Thread routines for corrections in rocket
 */
void *gyroscope_thread(void *arg)
{
    arg_gyros *info = arg;
    printf("Nació hilo de correcniones giroscopio Nº %d\n", info->id);
    sem_t sem_gyro = info->sem_gyro;
    int *dir_gyros = info->men_gyro;
    int *on_gyro = info->on_gyro;
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    if (rc != 0)
    {
        perror("Error in set Thread Cancellation\n");
    }
    while (is_on)
    {
        pthread_testcancel();

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
            sleep(INTERVAL);
        }
        else
        {
            sem_post(&sem_gyro);
            pthread_create(&array_threads_signals[info->id], NULL, signal_gyro, arg);
            break;
        }
    }
}

void *pricipal_engine_thread()
{
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    if (rc != 0)
    {
        perror("Error in set Thread Cancellation\n");
    }
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_wait(&(sem_gs[i]));
        }
        sem_wait(&sem_gas);
        if (state_gyros[0] || state_gyros[1])
        {
            *gasoline += 0.5;
        }
        sem_post(&sem_gas);
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_post(&(sem_gs[i]));
        }
    }
}

void *go_up_30m()
{
    while (is_on)
    {
        sleep(INTERVAL);
        if (*distance < 30)
        {
            *distance += 2;
        }
        else
        {
            create_signals_gyros();
            pthread_create(&array_threads_signals[2], NULL, signal_restart_landing, NULL);
            break;
        }
    }
}

void *go_up_explode()
{
    while (is_on)
    {
        sleep(INTERVAL);
        sem_wait(&sem_gas);
        if (*gasoline > 5)
        {
            *distance += 2; //para compensar lo que resta el programa del profe
        }
        else
        {
            is_on = 0;
            pthread_cancel(array_threads[2]);
            pthread_cancel(main_id);
            break;
        }
        sem_post(&sem_gas);
    }
    printf("ROCKET HAS BEEN EXPLOTE!\n");
}

/*
 * Thread routines for check singnals 
 */
void *signal_gyro(void *arg)
{
    arg_gyros *info = arg;
    printf("Nacio hilo del signal Nº %d\n", info->id);
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    if (rc != 0)
    {
        perror("Error in set Thread Cancellation\n");
    }
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
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

void *signal_restart_landing()
{
    printf("Nacio hilo del signal Nº 3 (30 metros alarma)\n");
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    if (rc != 0)
    {
        perror("Error in set Thread Cancellation\n");
    }
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        if (
            *distance < 5 &&
            *gyros1 != 0 && *gyros2 != 0)
        {
            terminate_event_1();
            pthread_create(&array_threads[3], NULL, go_up_30m, NULL);
            break;
        }
    }
}

void *signal_explote_rocket()
{
    printf("Nacio hilo del signal Nº 4 (10por gasolina)\n");
    while (is_on)
    {
        sleep(INTERVAL);
        if (
            *gasoline < 10)
        {
            terminate_event_1();
            terminate_event_3();
            pthread_create(&array_threads[4], NULL, go_up_explode, NULL);
            break;
        }
    }
}
