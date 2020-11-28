#include <time.h>
#include <stdio.h>
#define __USE_GNU
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>

/* 
INTRUCCIONES

El programa requere del parámetro t que setea la cantidad de hilos para crear
Luego se mostrará un mensaje por consola para que se ingrese los números de arreglo
separados por comas

IMPORTANTE
Este programa setea hilos a cores específicos, se aplica un módulo de 4 para 
no preveer problemas si se crean mas de 4 hilos. Es decir, un quinto hilo se alojaría
en el primer core y un sexto hilo en el segundo core.
*/

#define BUFF 200
//#define _GNU_SOURCE

//aray global to short
int *array;

typedef struct arg_thread
{
    int core_id;
    int ind_left;
    int ind_right;
} arg_thread;

#define SUCCESS_MSG "Thread %lu setted to CPU %d\n"
#define FAILURE_MSG "Failed to set thread %lu in CPU %d\n"

void merge(int low, int half, int high)
{

    int left_block = half - low + 1;
    int right_block = high - half;

    int *left = calloc(left_block, sizeof(int));
    int *right = calloc(right_block, sizeof(int));

    for (int i = 0; i < left_block; i++)
    {
        left[i] = array[i + low];
    }

    for (int i = 0; i < right_block; i++)
    {
        right[i] = array[i + half + 1];
    }

    int ind_array = low;

    int i = 0;
    int j = 0;

    while (i < left_block && j < right_block)
    {
        if (left[i] <= right[j])
            array[ind_array++] = left[i++];
        else
            array[ind_array++] = right[j++];
    }

    while (i < left_block)
    {
        array[ind_array++] = left[i++];
    }

    while (j < right_block)
    {
        array[ind_array++] = right[j++];
    }

    free(left);
    free(right);
}

void merge_sort(int low, int high)
{
    // calculating mid point of array
    int half = low + (high - low) / 2;

    if (low < high)
    {
        // calling first half
        merge_sort(low, half);

        // calling second half
        merge_sort(half + 1, high);

        // merging the two halves
        merge(low, half, high);
    }
}

void *thread_routine(void *arg)
{
    arg_thread *info = arg;

    const pthread_t pid = pthread_self();
    const int core_id = info->core_id % 4;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    const int aff_result = pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset);
    
    char *buffer;
    if (CPU_ISSET(core_id, &cpuset))
    {   
        const int space = snprintf(NULL, 0, SUCCESS_MSG, pid, core_id);
        buffer = malloc(space);
        snprintf(buffer, space, SUCCESS_MSG, pid, core_id);
    }else
    {
        const size_t space = snprintf(NULL, 0, FAILURE_MSG, pid, core_id);
        buffer = malloc(space);
        snprintf(buffer, space, FAILURE_MSG, pid, core_id);
    }
    
    int low;
    int high;

    low = info->ind_left;
    high = info->ind_right;

    int half = low + (high - low) / 2;

    if (low < high)
    {
        merge_sort(low, half);
        merge_sort(half + 1, high);
        merge(low, half, high);
    }

    return buffer;
}

int main(int argc, char *argv[])
{
    char *cp;
    arg_thread *tsk;

    int LENGTH_ARRAY = 0;
    int NUMBER_THREADS = 4;

    char opcion;

    while ((opcion = getopt(argc, argv, "t:")) != -1)
    {
        switch (opcion)
        {
        case 't':
            NUMBER_THREADS = atoi(optarg);
            break;
        default:
            printf("%s", "Ingrese el parametro n con un número a usar como cantidad de hilos\n");
            break;
        }
    }

    printf("Ingrese los números separados por comas:");

    char ch[BUFF];
    int exist_array = 0;
    while (((fgets(ch, BUFF, stdin)) != NULL))
    {
        if (strcmp(ch, "\n") != 0 && strlen(ch) != 1)
        {
            char *input = strtok(ch, "\n");

            char ch2[BUFF];
            strcpy(ch2, input);

            LENGTH_ARRAY = 0;
            char *b = strtok(ch2, ",");
            while (b != NULL)
            {
                LENGTH_ARRAY++;
                b = strtok(NULL, ",");
            }
            array = (int *)calloc(LENGTH_ARRAY, sizeof(int));
            char *p = strtok(ch, ",");
            int i = 0;
            while (p != NULL)
            {
                array[i] = atoi(p);
                p = strtok(NULL, ",");
                i++;
            }
            exist_array = 1;
        }
        if (exist_array)
        {
            break;
        }
        printf("Ingrese los números separados por comas:");
    }

    printf("Initial array:");
    for (int i = 0; i < LENGTH_ARRAY; i++)
    {
        printf(" %d", array[i]);
    }

    pthread_t array_threads[NUMBER_THREADS];
    arg_thread arg_list[NUMBER_THREADS];
    int len = LENGTH_ARRAY / NUMBER_THREADS;

    int low = 0;
    for (int i = 0; i < NUMBER_THREADS; i++, low += len)
    {
        arg_thread *arg_t = &arg_list[i];
        arg_t->core_id = i;
        arg_t->ind_left = low;
        arg_t->ind_right = low + len - 1;
        if (i == (NUMBER_THREADS - 1))
            arg_t->ind_right = LENGTH_ARRAY - 1;
    }

    for (int i = 0; i < NUMBER_THREADS; i++)
    {
        arg_thread *arg_t = &arg_list[i];
        pthread_create(&array_threads[i], NULL, thread_routine, arg_t);
    }

    printf("\n");
    for (int i = 0; i < NUMBER_THREADS; i++)
    {   
        void *msg_thread;
        int result = pthread_join(array_threads[i], &msg_thread);
        if(result != 0){
            printf("Error in: pthread_join");
        }
        
        printf("%s\n", (char *)msg_thread);
        free(msg_thread);
    }

    arg_thread *arg_left = &arg_list[0];
    for (int i = 1; i < NUMBER_THREADS; i++)
    {
        arg_thread *arg_right = &arg_list[i];
        merge(arg_left->ind_left, arg_right->ind_left - 1, arg_right->ind_right);
    }

    printf("\nShorted array:");
    for (int i = 0; i < LENGTH_ARRAY; i++)
    {
        printf(" %d", array[i]);
    }
    printf("\n");

    return 0;
}