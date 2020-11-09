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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int fds[2];
    char *argv[3];
    pipe(fds);                         // create pipe
    if (fork() == 0) { //reads
		close(0); //close stdin
		dup(fds[0]);  // redirect standard input to the pipe table
		
		argv[0] ="/bin/more";argv[1] = 0;
		execv(argv[0], argv);
		close(fds[0]);
		close(fds[1]); //closed unused fids
		exit(0);
	} else { //writes
       close(1); //close the stdout
	   dup(fds[1]);          // redirect standard output to the pipe table;
       
	   argv[0] = "/bin/ps"; argv[1] =  0;
       execv(argv[0], argv);
	   close(fds[0]);
	   close(fds[1]); //closed unused fids
	   exit(0);
    }
}
