#include <pthread.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct Hilo
{
    int id;
    struct timeval child_time;
    struct timeval parent_time;
    long *times;
} Hilo;

void * getTime(void *idA){

    Hilo *ar = (Hilo *) idA;
    gettimeofday(&ar->child_time, NULL);
    ar->times[ar->id] = (ar->child_time.tv_usec) - (ar->parent_time.tv_usec);
}

int main(int argc, char *argv[]){

    char opcion;

    int num_hilos = -1;
    while ((opcion = getopt(argc, argv, "n:")) != -1)
    {
        switch (opcion)
        {
        case 'n':
            num_hilos = atoi(optarg);
            break;
        default:
            printf("%s", "Ingrse el parametro s con un número a usar como firma en sus mensajes\n");
            break;
        }
    }
    if (num_hilos < 0)
    {
        printf("Ingrese un valor para el parámetro n mayor a 0");
        exit(1);
    }

    long *times = calloc(num_hilos, sizeof(pthread_t));


    pthread_t *tids = calloc(num_hilos, sizeof(pthread_t));

    for (int i = 0; i < num_hilos; i++)
    {
        Hilo *ar = (Hilo *)calloc(1, sizeof(Hilo));
        ar->id = i;
        ar->times = times;
        gettimeofday(&ar->parent_time, NULL);
        pthread_create(&tids[i], NULL, getTime, (void *)ar);
    }

    for (int i = 0; i < num_hilos; i++)
    {
        pthread_join(tids[i], NULL);
    }

    long acum = 0;
    for (long i = 0; i < num_hilos; i++)
    {
        acum += times[i];
    }

    double prom = acum / num_hilos;
    printf("El tiempo promedio fue de: %f", prom);
    return 0;
}