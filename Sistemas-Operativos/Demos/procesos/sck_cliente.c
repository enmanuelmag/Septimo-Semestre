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
#include <sys/stat.h>
#include <fcntl.h>


#define len_buff 1000
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
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) { 
		perror("Socket create failed.\n") ; 
		return -1 ; 
	} 
	
	//Name the socket as agreed with server.
	address.sin_family = AF_INET;
	// direccion donde esta ubicado el servidor
	address.sin_addr.s_addr = inet_addr("186.3.245.44");
	address.sin_port = htons(7734);
	len = sizeof(address);

	result = connect(sockfd, (struct sockaddr *)&address, len);
	if(result == -1)
	{
		perror("Error has occurred");
		exit(-1);
	}

	char *path_dst = argv[1];
	printf("%s\n", path_dst);

	int destino = open(path_dst, O_RDONLY | O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, "666");
	if (destino == -1)
	{
		perror("Error en el open del archivo destino");
		exit(0);
	}

	char *buff = (char *)calloc(len_buff, sizeof(char));
	int total = 0;
	int numByt;
	while ((numByt = read(sockfd, buff, len_buff)) != 0)
	{
		total += numByt;
		write(destino, buff, numByt);
		free(buff);
		buff = (char *)calloc(len_buff, sizeof(char));
	}

	close(sockfd);

	exit(0);
}
