
#include <pthread.h>
#include <stdio.h>

int sum = 0, scope, i; /* this data is shared by the thread(s) */
int NUM_THREADS = 5;
void *runner(void *param); /* the thread */

int main(int argc, char *argv[])
{
	pthread_t tid;				 /* the thread identifier */
	pthread_t tida[NUM_THREADS]; /* the thread identifier */
	pthread_attr_t attr;		 /* set of attributes for the thread */

	if (argc != 2)
	{
		fprintf(stderr, "usage: a.out <integer value>\n");
		return -1;
	}

	if (atoi(argv[1]) < 0)
	{
		fprintf(stderr, "Argument %d must be non-negative\n", atoi(argv[1]));
		return -1;
	}

	/* get the default attributes */
	pthread_attr_init(&attr);

	// solo 1 thread
	// create the thread
	pthread_create(&tid, &attr, runner, argv[1]);

	// now wait for the thread to exit
	pthread_join(tid, NULL);

	printf("sum = %d\n", sum);
	/*
//multiples threads con diferentes sincronicaciones

for (i=1;i<NUM_THREADS;i++)
{
	pthread_create(&tida[i],&attr,runner,argv[1]);
}

for (i=1;i<NUM_THREADS;i++)
{
	pthread_join(tida[i],NULL);
}

printf("sum = %d\n",sum);*/
	return (0);
}

/**
 * The thread will begin control in this function
 */
void *runner(void *param)
{
	int i, upper = atoi(param);

	if (upper > 0)
	{
		for (i = 1; i <= upper; i++)
			sum += i;
	}
	sleep(60);
	pthread_exit(0);
}
