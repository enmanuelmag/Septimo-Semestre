#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

#define len_buff 1000

typedef struct Hilo
{
    int id;
    struct timeval child_time;
    struct timeval parent_time;
    long *times;
} Hilo;

void *getTime(void *idA)
{

    Hilo *ar = (Hilo *)idA;
    gettimeofday(&ar->child_time, NULL);
    ar->times[ar->id] = (ar->child_time.tv_usec) - (ar->parent_time.tv_usec);
}

int main(int argc, char *argv[])
{

    char opcion;

    int num_proc = -1;
    while ((opcion = getopt(argc, argv, "n:")) != -1)
    {
        switch (opcion)
        {
        case 'n':
            num_proc = atoi(optarg);
            break;
        default:
            printf("%s", "Ingrse el parametro s con un número a usar como firma en sus mensajes\n");
            break;
        }
    }
    if (num_proc < 0)
    {
        printf("Ingrese un valor para el parámetro n mayor a 0");
        exit(1);
    }

    long acum_time = 0;

    for (int i = 0; i < num_proc; i++)
    {
        struct timeval parent_time;
        struct timeval child_time;
        int fds[2]; // descriptores, 0 para stdin, 0 para stdout
        pipe(fds);
        gettimeofday(&parent_time, NULL);
        int pid = fork();
        if (pid == 0)
        {
            gettimeofday(&child_time, NULL);
            //printf("%ld\n", child_time.tv_usec);
            dup2(fds[1], 1); // redirijo el stdout
            printf("%ld", child_time.tv_usec);
            close(fds[0]);
            close(fds[1]); //closed unused fids
            return 0;
        }
        else
        {
            int stdin_copy = dup(0);
            dup2(fds[0], 0);
            int status;
            waitpid(pid, &status, 0);
            char *buff_p = (char *)calloc(len_buff, sizeof(char));
            read(fds[0], buff_p, len_buff);
            //printf("%ld\n", atol(buff_p));
            //printf("PP %ld\n", parent_time.tv_usec);
            acum_time += atol(buff_p) - (parent_time.tv_usec);
            close(fds[0]);
            close(fds[1]); //closed unused fids
            free(buff_p);
            dup2(stdin_copy, 0);
        }
    }
    double prom = acum_time / num_proc;
    printf("EL tiempo promedio de los procesos hijos fue: %f\n", prom);

    long *times = calloc(num_proc, sizeof(pthread_t));

    pthread_t *tids = calloc(num_proc, sizeof(pthread_t));

    for (int i = 0; i < num_proc; i++)
    {
        Hilo *ar = (Hilo *)calloc(1, sizeof(Hilo));
        ar->id = i;
        ar->times = times;
        gettimeofday(&ar->parent_time, NULL);
        pthread_create(&tids[i], NULL, getTime, (void *)ar);
    }

    for (int i = 0; i < num_proc; i++)
    {
        pthread_join(tids[i], NULL);
    }

    long acum = 0;
    for (long i = 0; i < num_proc; i++)
    {
        acum += times[i];
    }

    prom = acum / num_proc;
    printf("El tiempo promedio de los hilos fue: %f\n", prom);

    return 0;
}