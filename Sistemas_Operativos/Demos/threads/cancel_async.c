/*
 * cancel_async.c
 *
 * Demonstrate asynchronous cancellation of a compute-bound
 * thread.
 *
 * Special notes: On a Solaris 2.5 uniprocessor, this test will
 * hang unless a second LWP is created by calling
 * thr_setconcurrency() because threads are not timesliced.
 */
#include <pthread.h>
#include "errors.h"

#define SIZE    10      /* array size */

static int matrix[SIZE][SIZE];


void print_array (int matrix[SIZE][SIZE])
{
    int i, j;
    int first;

    for (i = 0; i < SIZE; i++) {
        printf ("[");
        first = 1;
        for (j = 0; j < SIZE; j++) {
            if (!first)
                printf (",");
            printf ("%x", matrix[i][j]);
            first = 0;
        }
        printf ("]\n");
    }
        
}

/*
 * Loop until cancelled. The thread can be cancelled at any
 * point within the inner loop, where asynchronous cancellation
 * is enabled. The loop multiplies the two matrices matrixa
 * and matrixb.
 */
void *thread_routine (void *arg)
{
    int cancel_type, status;
    int i, j, k, value = 1;
    

    status = pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS,&cancel_type);
        if (status != 0)
            err_abort (status, "Set cancel type");

    /*
     * Initialize the matrices to something arbitrary.
     */
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
            matrix[i][j] = 0;
        }

    while (1) {
        /*
         * compute the negative of the matrix
         */
       
        for (i = 0; i < SIZE; i++)
            for (j = 0; j < SIZE; j++) {
                if (matrix[i][j] == 0)
			matrix[i][j] =1;
		else	
			matrix[i][j] =0;
            }
    }
}

int main (int argc, char *argv[])
{
    pthread_t thread_id;
    void *result;
    int status;

    status = pthread_create (&thread_id, NULL, thread_routine, NULL);
    if (status != 0)
        err_abort (status, "Create thread");
    sleep (1);
    status = pthread_cancel (thread_id);
    if (status != 0)
        err_abort (status, "Cancel thread");
    status = pthread_join (thread_id, &result);
    if (status != 0)
        err_abort (status, "Join thread");
    if (result == PTHREAD_CANCELED)
        printf ("Thread cancelled\n");
    else
        printf ("Thread was not cancelled\n");
    printf ("Matrix :\n");
    print_array (matrix);
    return 0;
}
