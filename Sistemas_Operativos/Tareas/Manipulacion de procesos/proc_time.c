#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define len_buff 1000

int main(int argc, char *argv[]){

    
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
        }else{
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
    printf("EL tiempo promedio de los hijos fue: %f\n", prom);

    return 0;
}