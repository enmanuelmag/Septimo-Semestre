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
///   SERVER   

int main(int argc, char *argv[])
{
	char *path_src = argv[1];

	int origen = open(path_src, O_RDONLY, "666");
	if (origen == -1)
	{
		perror("Error al abrir el archivo de origen");
		exit(0);
	}

	//Declaring process variables.
	int server_sockfd, client_sockfd;
	int server_len ; 
	int rc ; 
	unsigned client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	//Remove any old socket and create an unnamed socket for the server.
	// La funcion socket devuevle un FD del socket creado. 
	// Este lo uso el cliente unsa vez la conexion se realizó anteriormente
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(7734) ; 
	server_len = sizeof(server_address);

	// Hace que el socket se conecte con la direccion
	rc = bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	printf("RC from bind = %d\n", rc ) ; 
	
	//Create a connection queue and wait for clients
	// Le dice que este socket es pasivo, estara escuhcnado
	rc = listen(server_sockfd, 5);
	printf("RC from listen = %d\n", rc ) ; 

	client_len = sizeof(client_address);
	// Para aceptar conexiones
	client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
	printf("after accept()... client_sockfd = %d\n", client_sockfd) ; 

	while(1)
	{
		char ch;
		printf("server waiting\n");

		char *buff = (char *)calloc(len_buff, sizeof(char));
		int total = 0;
		int numByt;
		while ((numByt = read(origen, buff, len_buff)) != 0)
		{
			total += numByt;
			write(client_sockfd, buff, numByt);
			free(buff);
			buff = (char *)calloc(len_buff, sizeof(char));
			printf("Se envió %d\n", total);
		}
		printf("Se envió la fotito\n");
		break;
	}

	printf("server exiting\n");

	close(client_sockfd);
	return 0;
}
