#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define buff 200
#define len_buff 1000

void freeM(char ***vect, int filas)
{

    for (long long i = 0; i < filas; i++)
    {
        free((*vect)[i]);
    }
    free(*vect);
}

int main()
{

    char ch[buff];
    int fds[2];
    char *argv[3];
    printf("$ ");

    pipe(fds);
    close(fds[1]);

    while (((fgets(ch, buff, stdin)) != NULL))
    {

        if (strcmp(ch, "\n") != 0 && strlen(ch) != 1)
        {
            char *ruta = strtok(ch, "\n");

            if (strcmp(ruta, "exit") == 0)
            {
                exit(0);
            }

            int pid = fork();

            if (pid == 0)
            {

                dup2(fds[1], 1); // redirect standard output to the pipe table;
                if (strstr(ch, " ") != NULL)
                {
                    char ch2[buff];
                    strcpy(ch2, ch);
                    int size = 1;
                    char *b = strtok(ch2, " ");
                    while (b != NULL)
                    {
                        size++;
                        b = strtok(NULL, " ");
                    }
                    char **parameters = (char **)calloc(size + 1, sizeof(char *));
                    char *p = strtok(ch, " ");
                    int i = 0;

                    while (p != NULL)
                    {
                        parameters[i] = (char *)calloc(strlen(p) + 1, sizeof(char));
                        strcpy(parameters[i], p);
                        p = strtok(NULL, " ");
                        i++;
                    }
                    if (parameters[0] != NULL)
                    {
                        execvp(parameters[0], parameters);
                    }
                    if (ruta != NULL)
                    {
                        printf("%s: el no fue encontrado\n", ruta);
                    }
                    else
                    {
                        printf("la ruta no fue encontrado\n");
                    }
                    freeM(&parameters, size);
                }
                else
                {
                    execlp(ruta, ruta, (void *)0);
                    printf("%s: el no fue encontrado\n", ruta);
                }
                close(fds[0]);
                close(fds[1]);
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
                printf("%s", buff_p);
                
                free(buff_p);
                dup2(stdin_copy, 0);
            }
        }
        printf("$ ");
    }
    close(fds[0]);
    return 0;
}