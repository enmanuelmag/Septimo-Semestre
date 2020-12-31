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

#define NUM 5
#define SHMSZ 4
#define NUM_GYROS 2
#define CPU_CORES 4
#define NUM_EVENTS 3
#define SHM_ADDR 233
#define NUM_THREADS 5
#define NUM_THREADS_SIGNALS 5
#define SUCCESS_MSG "Hilo '%s %d' seteado en el CPU %d\n"
#define FAILURE_MSG "Error al setear el hilo '%s %d' en CPU %d\n"

//STRUCTURES
typedef struct arg_gyros
{
    int id;
    int *on_gyro;
    int *men_gyro;
    sem_t sem_gyro;
} arg_gyros;

//GLOBAL VARIABLES
int INTERVAL = 1;
int ret;
int newprio = 1;
sem_t sem_gas;
sem_t sem_gs[NUM_GYROS];
int is_on = 1;
int shmid[NUM];
int landscape = 0;
int princp_engine = 1;
int state_gyros[NUM_GYROS];
int *param[NUM], *distance, *gasoline, *gyros1, *gyros2, *alarma;

arg_gyros array_gyros[NUM_GYROS];
pthread_t main_id;
pthread_t array_threads[NUM_THREADS];
pthread_t array_threads_signals[NUM_THREADS_SIGNALS];

//USER'S FUNCTIONS DEFINITION
void handle_cod_101();
void handle_cod_102();
void handle_cod_103();
void terminate_event_1();
void terminate_event_3();
void restart_thrusters();
void create_signals_gyros();
int init_shared_memory(void);
void restart_principal_engine();
void create_pe_thread();
void create_rest_thread();
void create_explo_thread();
void create_land_thread();
void sig_handler_threads(int signo);

//THREAD'S ROUTINES DEFINITION
void *go_up_30m();
void *go_up_explode();
void *signal_gyro(void *arg);
void *signal_landing_check();
void *signal_explote_rocket();
void *signal_restart_landing();
void *pricipal_engine_thread();
void *gyroscope_thread(void *arg);

//IMPLEMETATION
int main(int argc, char *argv[])
{
    main_id = pthread_self();
    for (int i = 0; i < NUM_GYROS; i++)
    {
        sem_init(&(sem_gs[i]), 0, 1);
    }

    sem_init(&sem_gas, 0, 1);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (init_shared_memory() == -1)
    {
        printf("Error en init memory\n");
    }

    printf("Init...\n");
    create_signals_gyros(); //gyroscope_thread check

    create_pe_thread();    //check
    create_rest_thread();  //hijo go_up_30m check
    create_explo_thread(); //hijo go_up_explode (el hijo si sera bajo demanda)
    create_land_thread();  //

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
        pthread_attr_t tattr;
        struct sched_param s_param;
        ret = pthread_attr_init(&tattr);
        ret = pthread_attr_getschedparam(&tattr, &s_param);
        s_param.sched_priority = newprio;
        ret = pthread_attr_setschedparam(&tattr, &s_param);
        arg_gyros *arg = &array_gyros[i];
        arg->id = i;
        arg->men_gyro = param[i + 2];
        arg->sem_gyro = sem_gs[i];
        state_gyros[i] = 0;
        arg->on_gyro = &(state_gyros[i]);

        int res;
        res = pthread_create(&array_threads_signals[i], &tattr, signal_gyro, arg);
        if (res != 0)
        {
            perror("Creation of thread failed");
            exit(EXIT_FAILURE);
        }
        res = pthread_create(&array_threads[i], NULL, gyroscope_thread, arg);
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

void create_pe_thread()
{
    pthread_attr_t tattr;
    struct sched_param s_param;
    ret = pthread_attr_init(&tattr);
    ret = pthread_attr_getschedparam(&tattr, &s_param);
    s_param.sched_priority = newprio;
    ret = pthread_attr_setschedparam(&tattr, &s_param);
    pthread_create(&array_threads[2], NULL, pricipal_engine_thread, NULL);
}

void create_rest_thread()
{
    pthread_attr_t tattr_rest;
    struct sched_param s_param_rest;
    ret = pthread_attr_init(&tattr_rest);
    ret = pthread_attr_getschedparam(&tattr_rest, &s_param_rest);
    s_param_rest.sched_priority = newprio;
    ret = pthread_attr_setschedparam(&tattr_rest, &s_param_rest);
    pthread_create(&array_threads_signals[2], &tattr_rest, signal_restart_landing, NULL);

    pthread_create(&array_threads[3], NULL, go_up_30m, NULL);
}

void create_explo_thread()
{
    pthread_attr_t tattr_expl;
    struct sched_param s_param_expl;
    ret = pthread_attr_init(&tattr_expl);
    ret = pthread_attr_getschedparam(&tattr_expl, &s_param_expl);
    s_param_expl.sched_priority = newprio;
    ret = pthread_attr_setschedparam(&tattr_expl, &s_param_expl);
    pthread_create(&array_threads_signals[3], &tattr_expl, signal_explote_rocket, NULL);
}

void create_land_thread()
{
    pthread_attr_t tattr_land;
    struct sched_param s_param_land;
    ret = pthread_attr_init(&tattr_land);
    ret = pthread_attr_getschedparam(&tattr_land, &s_param_land);
    s_param_land.sched_priority = newprio;
    ret = pthread_attr_setschedparam(&tattr_land, &s_param_land);
    pthread_create(&array_threads_signals[4], &tattr_land, signal_landing_check, NULL);
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

void manage_principal_engine(int force_restart)
{
    if (force_restart)
    {
        pthread_kill(array_threads[2], SIGTSTP);
        princp_engine = 0;
    }
    if (princp_engine == 0)
    {
        princp_engine = 1;
        pthread_kill(array_threads[2], SIGCONT);
    }
}

void restart_thrusters()
{
    terminate_event_1();
    create_signals_gyros();
}

void sig_handler_threads(int signo)
{
    int i;
    if (signo == SIGCONT)
    {
        printf("Desperté\n");
        return;
    }
    else if (signo == SIGTSTP)
    {
        pause();
        return;
    }
    return;
}

//Thread routines for corrections in rocket
void *gyroscope_thread(void *arg)
{
    if (signal(SIGCONT, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGCONT\n");
    if (signal(SIGTSTP, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGTSTP\n");
    printf("Creado\n");
    arg_gyros *info = arg;
    const pthread_t pid = pthread_self();
    const int core_id = info->id % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "CORRECCION GYRO", info->id, core_id);
    }
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
    pause();
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
            pthread_kill(array_threads_signals[info->id], SIGCONT);
            pause();
        }
    }
}

void *pricipal_engine_thread()
{
    if (signal(SIGCONT, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGINT\n");
    if (signal(SIGTSTP, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGTSTP\n");

    const pthread_t pid = pthread_self();
    const int core_id = 4 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "P.E.", 4, core_id);
    }
    else
    {
        printf(SUCCESS_MSG, "P.E.", 4, core_id);
    }
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
    if (signal(SIGCONT, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGINT\n");
    if (signal(SIGTSTP, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGTSTP\n");

    const pthread_t pid = pthread_self();
    const int core_id = 2 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "SUBIR (30 m)", 2, core_id);
    }
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    pause();
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        if (*distance < 30)
        {
            *distance += 2;
        }
        else
        {
            create_signals_gyros();
            pthread_kill(array_threads_signals[2], SIGCONT);
            pause();
        }
    }
}

void *go_up_explode()
{
    const pthread_t pid = pthread_self();
    const int core_id = 3 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "ABORTAR (Gas 10)", 3, core_id);
    }
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    while (is_on)
    {
        pthread_testcancel();
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

//Thread routines for check singnals
void *signal_gyro(void *arg)
{
    arg_gyros *info = arg;
    const pthread_t pid = pthread_self();
    const int core_id = info->id % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "SIGNAL GYRO", info->id, core_id);
    }
    else
    {
        printf(SUCCESS_MSG, "SINGAL GYRO", info->id, core_id);
    }

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
        if (*distance <= 5)
        {
            break;
        }
        sleep(INTERVAL);
        if (*(info->men_gyro))
        {
            int res, err;
            printf("ID: %d", info->id);
            if (pthread_kill(array_threads[info->id], SIGCONT) == 0)
            {
                printf("Wake\n");
            }
            else
            {
                printf("Error\n");
            }
            pause();
        }
    }
}

void *signal_restart_landing()
{
    const pthread_t pid = pthread_self();
    const int core_id = 2 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "SIGNAL (30 m)", 2, core_id);
    }
    else
    {
        printf(SUCCESS_MSG, "SIGNAL (30 m)", 2, core_id);
    }
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
            (*gyros1 != 0 || *gyros2 != 0))
        {
            terminate_event_1();
            manage_principal_engine(0);
            pthread_kill(array_threads[3], SIGCONT);
            pause();
        }
    }
}

void *signal_explote_rocket()
{
    const pthread_t pid = pthread_self();
    const int core_id = 3 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "SIGNAL (Gas 10)", 3, core_id);
    }
    else
    {
        printf(SUCCESS_MSG, "SIGNAL (Gas 10)", 3, core_id);
    }
    int oldtype, rc;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        if (*gasoline < 10)
        {
            manage_principal_engine(0);
            terminate_event_1();
            terminate_event_3();
            pthread_create(&array_threads[4], NULL, go_up_explode, NULL);
            break;
        }
    }
}

void *signal_landing_check()
{
    const pthread_t pid = pthread_self();
    const int core_id = 5 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "SIGNAL (1 m)", 5, core_id);
    }
    else
    {
        printf(SUCCESS_MSG, "SIGNAL (1 m)", 5, core_id);
    }
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
        if (*distance == 0 &&
            *gyros1 == 0 && *gyros2 == 0)
        {
            is_on = 0;
            terminate_event_3();
            pthread_cancel(array_threads_signals[3]);
            pthread_cancel(main_id);
            printf("SUCCESSFUL LANDING\n");
            break;
        }
        if (
            *distance <= 1 &&
            (*gyros1 == 0 ||
             *gyros2 == 0))
        {
            terminate_event_1();
            princp_engine = 0;
            pthread_kill(array_threads[2], SIGTSTP);
        }
    }
}
