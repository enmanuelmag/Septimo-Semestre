/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Daniel Ochoa Donoso 2010 <dochoa@fiec.espol.edu.ec>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[])
{

    int c;
    int shmid;
    key_t key;
    char *shm, *s;
    int LENGTH_MSG = 20;
    int SHMSZ = 21;

    int cant;
    char opcion;
    char sign = -1;
    while ((opcion = getopt(argc, argv, "s:")) != -1)
    {

        switch (opcion)
        {
        case 's':
            sign = atoi(optarg);
            break;
        default:
            printf("%s", "Ingrse el parametro s con un número a usar como firma en sus mensajes\n");
            break;
        }
    }

    if (sign < 0)
    {
        printf("%s", "No se admiten valores negativos para el parámetro s\n");
        return -1;
    }

    /*
     * We'll name our shared memory segment "5678".
     */
    key = 5679;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {

        if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
        {
            perror("shmget-create");
            return (1);
        }
        perror("shmget-locate");
        return (1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        return (1);
    }

    while (1)
    {
        char input[LENGTH_MSG];
        printf("%s", "Ingrese su mensaje:");
        /* scanf(" %[^\t\n]s", input); */
        fgets(input, LENGTH_MSG, stdin);

        /* printf("%s %s", "El mensaje:", input); */
        s = shm;
        *s++ = sign;
        for (int c = 0; c < LENGTH_MSG; c++)
        {
            *s++ = input[c];
        }

        s = shm;
        while (*s == sign)
        {
        }

        s++;
        printf("%s", s++);
        s = shm;
    }

    return (0);
}
