/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Daniel Ochoa Donoso 2010 <daniel@daniel>
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

#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
 
 int main(int argc, char *argv[])
 {
	 pid_t cpid, w,cpid_array[5];
	 
	 int status,i;
	 
    for(i=0;i<5;i++){
		 cpid = fork();
		 if (cpid == -1) {
			 perror("fork");
			 exit(EXIT_FAILURE);
		 }
	 
		 if (cpid == 0) {            /* Code executed by child */
			 printf("Child PID is %ld and i is= %d \n", (long) getpid(),i);
			 sleep(10);
			 exit(atoi(argv[1]));
		}
	 	else {                    /* Code executed by parent */
		 cpid_array[i]=cpid;
	 	}
	 }
  	for(i=0;i<5;i++){
 		fprintf(stderr,"Waiting in the father %ld for child %ld\n",(long) getpid(),cpid_array[i]);
	    w = waitpid(cpid_array[i], &status, WUNTRACED | WCONTINUED);
		fprintf(stderr,"Child finished \n");
	}
    fprintf(stderr,"father finished \n");			 
	exit(EXIT_SUCCESS);
 }
