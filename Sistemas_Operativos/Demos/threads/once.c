/*
 * once.c
 *
 * Demonstrate the use of pthread_once() one-time
 * initialization.
 */
#include <pthread.h>
#include "errors.h"

pthread_once_t once_block = PTHREAD_ONCE_INIT;
pthread_mutex_t mutex;

/*
 * This is the one-time initialization routine. It will be
 * called exactly once, no matter how many calls to pthread_once
 * with the same control structure are made during the course of
 * the program.
 */
void once_init_routine (void)
{
        printf ("In once_init_routine.\n");
}

/*
 * Thread start routine that calls pthread_once.
 */
void *thread_routine (void *arg)
{
    int status;

    status = pthread_once (&once_block, once_init_routine);
    printf ("thread_routine.\n");
    return NULL;
}

int main (int argc, char *argv[])
{
    pthread_t thread_id;
    char *input, buffer[64];
    int status;

    status = pthread_create (&thread_id, NULL, thread_routine, NULL);
    status = pthread_once (&once_block, once_init_routine);
    printf ("In main\n");
    status = pthread_join (thread_id, NULL);
    return 0;
}
