/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) daniel  2010 <daniel@daniel>
 * 
 * cliente is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cliente is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


///   CLIENT   


int main(int argc, char *argv[])
{
	printf("This is the client program\n");

	int sockfd;
	int len, rc ;
	struct sockaddr_in address;
	int result;
	char ch = 'A';

   //Create socket for client.
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) { 
		perror("Socket create failed.\n") ; 
		return -1 ; 
	} 
	
	//Name the socket as agreed with server.
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(7734);
	len = sizeof(address);

	result = connect(sockfd, (struct sockaddr *)&address, len);
	if(result == -1)
	{
		perror("Error has occurred");
		exit(-1);
	}

	while ( ch < 'Y') {

		//Read and write via sockfd
		rc = write(sockfd, &ch, 1);
		if (rc == -1) break ; 
		read(sockfd, &ch, 1);
		printf("Char from server = %c\n", ch);		
	} 
	close(sockfd);

	exit(0);
}
