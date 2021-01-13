#include <time.h>
#include <stdio.h>
#include <string.h>
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
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>
//TODO fucnion para crear hilo go eplote con afnnida y cpu core
#define BUFF_MSG 999999
#define NUM 5
#define SHMSZ 4
#define MAXTRY 32
#define EVENT_1 1 //aterrizaje
#define EVENT_3 3 //reiniciando el aterrizaje
#define EVENT_4 4 //explotar cohete
#define EVENT_5 5 //apagando propulsores
#define NUM_GYROS 2
#define RATE_GO_UP 6
#define MASTER_PRIO 1
#define SLAVE_PRIO 3
#define NUM_EVENTS 3
#define SHM_ADDR 233
#define NUM_THREADS 5
#define NUM_SCK_THREAD 7
#define NUM_THREADS_SIGNALS 6
#define SUCCESS_MSG "Hilo '%s %d' seteado en el CPU %d\n"
#define FAILURE_MSG "Error al setear el hilo '%s %d' en CPU %d\n"

//STRUCTURES
struct sockaddr_in broadcastAddr;
typedef struct arg_gyros
{
    int id;
    int *on_gyro;
    int *men_gyro;
    sem_t sem_gyro;
} arg_gyros;

//GLOBAL VARIABLES
int CPU_CORES;
int CURRENT_ACTION = EVENT_1;
int INTERVAL = 1;
int manual_explote = 0;
int last_alamra = 0;
//int MASTER_PRIO = 1;
int msg_read = 0;
sem_t sem_read;
sem_t sem_gas;
sem_t sem_sock;
sem_t sem_dist;
sem_t sem_action;
int c_sck = 0;
sem_t sem_gs[NUM_GYROS];
sem_t sem_msg[NUM_SCK_THREAD];
int array_socks[NUM_SCK_THREAD];
int is_on = 1;
int shmid[NUM];
int landscape = 0;
int princp_engine = 1;
int state_gyros[NUM_GYROS];
int *param[NUM], distance, *dit_block, gasoline, *gyros1, *gyros2, *alarma;

arg_gyros array_gyros[NUM_GYROS];
pthread_t main_id;
pthread_t array_threads[NUM_THREADS];
pthread_t array_threads_signals[NUM_THREADS_SIGNALS];

//USER'S FUNCTIONS DEFINITION
void terminate_event_1(int all);
void terminate_event_3(int all);
void restart_thrusters();
void create_signals_gyros(int all);
int init_shared_memory(void);
void restart_principal_engine();
void create_pe_thread();
void terminte_program();
void create_rest_thread();
void create_explo_thread();
void create_land_thread();
void sig_handler_main(int signo);
void sig_handler_threads(int signo);
void create_signal_manual();
int create_sck_sender(int idx);
void change_action(int new_action);
//THREAD'S ROUTINES DEFINITION
void *go_up_30m();
void *signal_gyro(void *arg);
void *signal_landing_check();
void *signal_explote_rocket();
void *signal_restart_landing();
void *pricipal_engine_thread();
void *gyroscope_thread(void *arg);
void *sig_handler_manual();

//IMPLEMETATION
int main(int argc, char *argv[])
{
    if (signal(SIGINT, sig_handler_main) == SIG_ERR)
        printf("\nCan't catch SIGINT\n");

    printf("Iniciando...\n");
    CPU_CORES = get_nprocs();
    printf("Su computador dispone de %d cores\n", CPU_CORES);

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &broadcastAddr.sin_addr);
    broadcastAddr.sin_port = htons(8585);

    main_id = pthread_self();
    for (int i = 0; i < NUM_GYROS; i++)
    {
        sem_init(&(sem_gs[i]), 0, 1);
    }

    sem_init(&sem_read, 0, 1);
    sem_init(&sem_gas, 0, 1);
    sem_init(&sem_sock, 0, 1);
    sem_init(&sem_dist, 0, 1);
    sem_init(&sem_action, 0, 1);
    for (int i = 0; i < NUM_SCK_THREAD; i++)
    {
        sem_init(&sem_msg[i], 0, 0);
    }

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (init_shared_memory() == -1)
    {
        printf("Error en init memory\n");
    }

    create_signals_gyros(1);
    create_pe_thread();
    create_rest_thread();
    create_explo_thread();
    create_land_thread();
    create_signal_manual();

    while (is_on && !landscape)
    {
        sem_wait(&sem_read);
        if (msg_read == NUM_SCK_THREAD)
        {
            msg_read = 0;
            for (int i = 0; i < NUM_SCK_THREAD; i++)
            {
                sem_post(&sem_msg[i]);
            }
        }
        sem_post(&sem_read);
    }
}

void change_action(int new_action)
{
    sem_wait(&sem_action);
    CURRENT_ACTION = new_action;
    sem_post(&sem_action);
}

void terminte_program()
{
    //cerrar bloques de memoria y sockets
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_cancel(array_threads[i]);
        printf("--> Termino hilo slave %d\n", (i + 1));
    }
    printf("Hilos slaves finalizados\n");
    for (int i = 0; i < NUM_SCK_THREAD; i++)
    {
        sem_post(&sem_msg[i]);
    }
    for (int i = 0; i < NUM_THREADS_SIGNALS; i++)
    {
        pthread_cancel(array_threads_signals[i]);
        printf("--> Termino hilo master %d\n", (i + 1));
    }
    printf("Hilos masters finalizados\n");
    for (int i = 0; i < NUM_SCK_THREAD; i++)
    {
        close(array_socks[i]);
        printf("--> Cerro socket %d\n", (i + 1));
    }
    printf("Sockets cerrados\n");
    for (int i = 0; i < NUM; i++)
    {
        close(shmid[i]);
    }
    printf("Bloques de memoria cerrados\n");
    exit(EXIT_SUCCESS);
}

void create_signals_gyros(int all)
{
    for (int i = 0; i < NUM_GYROS; i++)
    {
        arg_gyros *arg = &array_gyros[i];
        arg->id = i;
        arg->men_gyro = param[i + 2];
        arg->sem_gyro = sem_gs[i];
        state_gyros[i] = 0;
        arg->on_gyro = &(state_gyros[i]);

        if (all == 1)
        {
            pthread_attr_t tattr;
            struct sched_param s_param;
            pthread_attr_init(&tattr);
            pthread_attr_getschedparam(&tattr, &s_param);
            s_param.sched_priority = MASTER_PRIO;
            pthread_attr_setschedparam(&tattr, &s_param);
            pthread_create(&array_threads_signals[i], &tattr, signal_gyro, arg);
        }

        pthread_attr_t tattr_slave;
        struct sched_param s_param_slave;
        pthread_attr_init(&tattr_slave);
        pthread_attr_getschedparam(&tattr_slave, &s_param_slave);
        s_param_slave.sched_priority = SLAVE_PRIO;
        pthread_attr_setschedparam(&tattr_slave, &s_param_slave);
        pthread_create(&array_threads[i], &tattr_slave, gyroscope_thread, arg);
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
    dit_block = param[0];
    distance = *param[0];
    gasoline = *param[1];
    gyros1 = param[2];
    gyros2 = param[3];
    alarma = param[4];
    return (1);
}

void create_pe_thread()
{
    pthread_attr_t tattr;
    struct sched_param s_param;
    pthread_attr_init(&tattr);
    pthread_attr_getschedparam(&tattr, &s_param);
    s_param.sched_priority = MASTER_PRIO;
    pthread_attr_setschedparam(&tattr, &s_param);
    array_threads[2] = 0;
    pthread_create(&array_threads[2], &tattr, pricipal_engine_thread, NULL);
}

void create_rest_thread()
{
    pthread_attr_t tattr_rest;
    struct sched_param s_param_rest;
    pthread_attr_init(&tattr_rest);
    pthread_attr_getschedparam(&tattr_rest, &s_param_rest);
    s_param_rest.sched_priority = MASTER_PRIO;
    pthread_attr_setschedparam(&tattr_rest, &s_param_rest);
    pthread_create(&array_threads_signals[2], &tattr_rest, signal_restart_landing, NULL);

    pthread_create(&array_threads[3], NULL, go_up_30m, NULL);
}

void create_explo_thread()
{
    pthread_attr_t tattr_expl;
    struct sched_param s_param_expl;
    pthread_attr_init(&tattr_expl);
    pthread_attr_getschedparam(&tattr_expl, &s_param_expl);
    s_param_expl.sched_priority = MASTER_PRIO;
    pthread_attr_setschedparam(&tattr_expl, &s_param_expl);
    pthread_create(&array_threads_signals[3], &tattr_expl, signal_explote_rocket, NULL);
}

void create_land_thread()
{
    pthread_attr_t tattr_land;
    struct sched_param s_param_land;
    pthread_attr_init(&tattr_land);
    pthread_attr_getschedparam(&tattr_land, &s_param_land);
    s_param_land.sched_priority = MASTER_PRIO;
    pthread_attr_setschedparam(&tattr_land, &s_param_land);
    pthread_create(&array_threads_signals[4], &tattr_land, signal_landing_check, NULL);
}

void create_signal_manual()
{
    pthread_attr_t tattr_land;
    struct sched_param s_param_land;
    pthread_attr_init(&tattr_land);
    pthread_attr_getschedparam(&tattr_land, &s_param_land);
    s_param_land.sched_priority = MASTER_PRIO;
    pthread_attr_setschedparam(&tattr_land, &s_param_land);
    pthread_create(&array_threads_signals[5], &tattr_land, sig_handler_manual, NULL);
}

void terminate_event_1(int all)
{
    pthread_cancel(array_threads[0]);
    pthread_cancel(array_threads[1]);
    if (all == 1)
    {
        pthread_cancel(array_threads_signals[0]);
        close(array_socks[0]);
        pthread_cancel(array_threads_signals[1]);
        close(array_socks[1]);
    }
}

void terminate_event_3(int all)
{
    pthread_cancel(array_threads[3]);
    if (all == 1)
    {
        pthread_cancel(array_threads_signals[2]);
    }
}

void manage_principal_engine(int force_restart)
{
    if (force_restart == 1)
    {
        //pthread_kill(array_threads[2], SIGTSTP);
        pthread_cancel(array_threads[2]);
        princp_engine = 0;
        create_pe_thread();
        princp_engine = 1;
    }
    else if (princp_engine == 0)
    {
        princp_engine = 1;
        pthread_kill(array_threads[2], SIGCONT);
    }
}

void restart_thrusters()
{
    terminate_event_1(1);
    create_signals_gyros(1);
}

void sig_handler_threads(int signo)
{
    int i;
    if (signo == SIGCONT)
    {
        return;
    }
    else if (signo == SIGTSTP)
    {
        pause();
        return;
    }
    return;
}

void sig_handler_main(int signo)
{
    if (signo == SIGINT)
    {
        terminte_program();
        return;
    }
    return;
}

int create_sck_sender(int idx)
{
    int broadcastSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcastSocket <= 0)
    {
        printf("No se pudo crear el socket\n");
        return -1;
    }

    // set socket options enable broadcast
    int broadcastEnable = 1;
    int ret_create = setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret_create)
    {
        printf("No se pudo abrir el socket en el modo broadcast\n");
        close(broadcastSocket);
        return -1;
    }
    array_socks[idx] = broadcastSocket;
    return broadcastSocket;
}

void send_mesg(int sckfd, char *msg, int len_msg)
{

    sem_wait(&sem_sock);
    int numsec;
    for (numsec = 1; numsec <= MAXTRY; numsec <<= 1)
    {
        int ret_send = sendto(sckfd, msg, len_msg, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
        if (ret_send > 0)
        {
            break;
        }
        else if (numsec <= MAXTRY / 2)
        {
            sleep(numsec);
        }
    }
    sem_wait(&sem_read);
    ++msg_read;
    sem_post(&sem_read);
    sem_post(&sem_sock);
}
//Thread routines for corrections in rocket
void *gyroscope_thread(void *arg)
{
    if (signal(SIGCONT, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGCONT\n");
    if (signal(SIGTSTP, sig_handler_threads) == SIG_ERR)
        printf("\nCan't catch SIGTSTP\n");
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
            gasoline -= 1;
            sem_post(&sem_gas);
            if (*dir_gyros > 0)
            {
                *dir_gyros -= 1;
            }
            else
            {
                *dir_gyros += 1;
            }
            sem_post(&sem_gyro);
            sleep(INTERVAL);
        }
        else
        {
            *on_gyro = 0;
            sem_post(&sem_gyro);
            //pthread_kill(array_threads_signals[info->id], SIGCONT);
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
    int sockfd = create_sck_sender(6);
    char *msg = "Combustible: %d\n--> Motor principal: %s - potencia %s\n";
    char *state;
    char *potencia;
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        sem_wait(&sem_dist);
        sem_wait(&sem_action);
        if (CURRENT_ACTION != EVENT_3 ||
            CURRENT_ACTION != EVENT_4)
        {
            distance -= 1;
            *dit_block = distance;
        }
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_wait(&(sem_gs[i]));
        }
        sem_wait(&sem_gas);

        if (CURRENT_ACTION == EVENT_3 ||
            CURRENT_ACTION == EVENT_4)
        {
            potencia = "acelerando";
            gasoline -= 5;
        }
        else if (state_gyros[0] || state_gyros[1])
        {
            potencia = "desacelerando";
            gasoline -= 0.5;
        }
        else
        {
            potencia = "normal";
            gasoline -= 1;
        }
        sem_post(&sem_gas);
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_post(&(sem_gs[i]));
        }
        sem_post(&sem_action);
        sem_post(&sem_dist);

        state = princp_engine ? "ON" : "OFF";
        int space = snprintf(NULL, 0, msg, gasoline, state, potencia);
        char *buffer = malloc(space);
        snprintf(buffer, space, msg, gasoline, state, potencia);
        send_mesg(sockfd, buffer, space);
        free(buffer);
        sem_wait(&sem_msg[6]);
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
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    pause();
    int time = 0;
    while (is_on)
    {
        pthread_testcancel();
        sleep(INTERVAL);
        sem_wait(&sem_dist);
        if (time < 5)
        {
            distance += RATE_GO_UP;
            ++time;
            sem_post(&sem_dist);
        }
        else
        {
            time = 0;
            sem_post(&sem_dist);
            create_signals_gyros(0);
            change_action(EVENT_1);
            pause();
        }
    }
}

//Thread routines for check singnals
void *signal_gyro(void *arg)
{
    arg_gyros *info = arg;
    const pthread_t pid = pthread_self();
    const int core_id = info->id % CPU_CORES;
    sem_t sem_gyro = info->sem_gyro;
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

    // Configure the port and ip we want to send to
    int sockfd = create_sck_sender(info->id);
    char *msg = "Giroscopio %d: %d (%s)\n";
    char *state;
    while (is_on)
    {
        pthread_testcancel();
        state = "OFF";
        sem_wait(&sem_action);
        sem_wait(&sem_gyro);
        if (
            CURRENT_ACTION == EVENT_1 &&
            *(info->men_gyro))
        {
            //sem_post(&sem_action);
            state = "ON - iniciando correcciones";
            int res, err;
            if (pthread_kill(array_threads[info->id], SIGCONT) != 0)
            {
                printf("Error\n");
            }
        }
        sem_post(&sem_action);
        int space = snprintf(NULL, 0, msg, info->id, *(info->men_gyro), state);
        char *buffer = malloc(space);
        snprintf(buffer, space, msg, info->id, *(info->men_gyro), state);
        sem_post(&sem_gyro);
        send_mesg(sockfd, buffer, space);
        free(buffer);
        sem_wait(&sem_msg[info->id]);
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
    int sockfd = create_sck_sender(2);
    char *msg = "Reiniciando aterrizaje";
    char *msg_alt = "Aterrizaje en curso";
    while (is_on)
    {
        pthread_testcancel();
        sem_wait(&sem_dist);
        sem_wait(&sem_action);
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_wait(&(sem_gs[i]));
        }
        if (
            CURRENT_ACTION != EVENT_4 &&
            distance < 5 &&
            (*gyros1 != 0 || *gyros2 != 0))
        {
            sem_post(&sem_action);
            change_action(EVENT_3);
            for (int i = 0; i < NUM_GYROS; i++)
            {
                sem_post(&(sem_gs[i]));
            }
            sem_post(&sem_dist);
            terminate_event_1(0);
            manage_principal_engine(0);
            pthread_kill(array_threads[3], SIGCONT);
            send_mesg(sockfd, msg, strlen(msg) + 1);
            sem_wait(&sem_msg[2]);
        }
        else
        {
            sem_post(&sem_action);
            for (int i = 0; i < NUM_GYROS; i++)
            {
                sem_post(&(sem_gs[i]));
            }
            sem_post(&sem_dist);
            send_mesg(sockfd, msg_alt, strlen(msg_alt) + 1);
            sem_wait(&sem_msg[2]);
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
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    int sockfd = create_sck_sender(3);
    char *msg = "Abortando aterrizaje";
    char *msg_alt = "Sin aborto de aterrizaje";
    while (is_on)
    {
        pthread_testcancel();
        sem_wait(&sem_gas);
        if (gasoline < 10 || manual_explote)
        {
            sem_post(&sem_gas);
            manual_explote = 0;
            send_mesg(sockfd, msg, strlen(msg) + 1);
            sem_wait(&sem_msg[3]);
            printf("El cohete ha explotado!\n");
            sleep(INTERVAL);
            pthread_kill(main_id, SIGINT);
        }
        else
        {
            sem_post(&sem_gas);
        }
        sem_wait(&sem_action);
        if (CURRENT_ACTION == EVENT_4)
        {
            sem_post(&sem_action);
            send_mesg(sockfd, msg, strlen(msg) + 1);
            sem_wait(&sem_msg[3]);
        }
        else
        {
            sem_post(&sem_action);
            send_mesg(sockfd, msg_alt, strlen(msg_alt) + 1);
            sem_wait(&sem_msg[3]);
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
    int sockfd = create_sck_sender(4);
    char *msg = "Distancia a tierra: %d %s\n";
    char *act;
    while (is_on)
    {
        pthread_testcancel();
        sem_wait(&sem_dist);
        int dist = distance;
        sem_post(&sem_dist);
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_wait(&(sem_gs[i]));
        }
        act = "";
        if (dist <= 0 &&
            *gyros1 == 0 && *gyros2 == 0)
        {
            is_on = 0;
            for (int i = 0; i < NUM_GYROS; i++)
            {
                sem_post(&(sem_gs[i]));
            }
            act = "<Aterrizaje satisfactorio>";
            int space = snprintf(NULL, 0, msg, dist, act);
            char *buffer = malloc(space);
            snprintf(buffer, space, msg, dist, act);
            send_mesg(sockfd, buffer, space);
            free(buffer);
            break;
        }
        if (
            dist <= 1 &&
            (*gyros1 == 0 ||
             *gyros2 == 0))
        {
            change_action(EVENT_5);
            terminate_event_1(0);
            princp_engine = 0;
            //pthread_kill(array_threads[2], SIGTSTP);
            act = "- apagando propulsores";
        }
        for (int i = 0; i < NUM_GYROS; i++)
        {
            sem_post(&(sem_gs[i]));
        }
        int space = snprintf(NULL, 0, msg, dist, act);
        char *buffer = malloc(space);
        snprintf(buffer, space, msg, dist, act);
        send_mesg(sockfd, buffer, space);
        free(buffer);
        sem_wait(&sem_msg[4]);
    }
    printf("Aterrizaje satisfactorio\n");
    sleep(INTERVAL);
    pthread_kill(main_id, SIGINT);
}

void *sig_handler_manual()
{
    const pthread_t pid = pthread_self();
    const int core_id = 6 % CPU_CORES;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    if (aff_result != 0)
    {
        printf(FAILURE_MSG, "SIGNAL (Alarm)", 6, core_id);
    }
    else
    {
        printf(SUCCESS_MSG, "SIGNAL (Alarm)", 6, core_id);
    }
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
    int sockfd = create_sck_sender(5);
    char *msg;
    char *msg_alt = "Sin alarmas";
    while (is_on)
    {
        pthread_testcancel();
        if (last_alamra != *alarma)
        {
            if (*alarma == 101)
            {
                manage_principal_engine(1);
                restart_thrusters();
                printf("*--------------------------*\n");
                msg = "Reiniciando todos los propulsores\n";
                send_mesg(sockfd, msg, strlen(msg) + 1);
            }
            else if (*alarma == 102)
            {
                manage_principal_engine(1);
                printf("*--------------------------*\n");
                msg = "Reiniciando propulsor principal\n";
                send_mesg(sockfd, msg, strlen(msg) + 1);
            }
            else if (*alarma == 103)
            {
                restart_thrusters();
                printf("*--------------------------*\n");
                msg = "Reiniciando propulsores de orientación\n";
                send_mesg(sockfd, msg, strlen(msg) + 1);
            }
            else if (*alarma == 104)
            {
                //manual_explote = 1;
                msg = "El cohete ha explotado!\n";
                send_mesg(sockfd, msg, strlen(msg) + 1);
                printf("El cohete ha explotado!\n");
                sleep(INTERVAL);
                pthread_kill(main_id, SIGINT);
                //terminte_program();
            }
            *alarma = 0;
            last_alamra = *alarma;
        }
        else
        {
            send_mesg(sockfd, msg_alt, strlen(msg_alt) + 1);
        }
        sem_wait(&sem_msg[5]);
    }
}
