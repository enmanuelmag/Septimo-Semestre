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

#define SHMSZ 21
#define LENGTH_MSG 20

int main()
{
    int c;
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     * We'll name our shared memory segment "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
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

    char sign = '0';

    while (1)
    {
        char input[LENGTH_MSG];
        printf("%s", "Ingrese su mensaje:");
        scanf("%s", input);
        int c;

        *s++ = sign;

        for (c = 0; c <= LENGTH_MSG; c++)
        {

            *s++ = input[c];
        }

        s -= (SHMSZ);
        while (*s != '0')
        {
            printf("%s", "Esperando...");
            sleep(1);
        }

        s++;

        for (int i = 0; i < LENGTH_MSG; i++)
        {
            printf("%s", s++);
        }

        s -= (SHMSZ);
    }

    return (0);
}
