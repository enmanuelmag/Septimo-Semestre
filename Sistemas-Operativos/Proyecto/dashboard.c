#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_MSG 999999
#define NUM_SCK_THREAD 7
#define LEN_MAX_STRING 350

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in broadcastStruc;
    unsigned short broadcastPort;
    char stringResponse[LEN_MAX_STRING + 1];
    int recvStringLen;

    broadcastPort = 8585;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("Error al crear el socket en el dashboard\n");
        exit(EXIT_FAILURE);
    }

    memset(&broadcastStruc, 0, sizeof(broadcastStruc));
    broadcastStruc.sin_family = AF_INET;
    broadcastStruc.sin_addr.s_addr = htonl(INADDR_ANY);
    broadcastStruc.sin_port = htons(broadcastPort);

    if (bind(sock, (struct sockaddr *)&broadcastStruc, sizeof(broadcastStruc)) < 0)
    {
        printf("No se pudo enlazar el socket con el puerto\n");
        exit(EXIT_FAILURE);
    }

    int msg_read = 0;

    while (1)
    {
        if ((recvStringLen = recvfrom(sock, stringResponse, LEN_MAX_STRING, 0, NULL, 0)) < 0)
        {
            printf("Error al leer mensaje del socket\n");
        }
        else
        {
            stringResponse[recvStringLen] = '\0';
            printf("--> %s\n", stringResponse); /* Print the received string */
            ++msg_read;
        }
        if (msg_read == NUM_SCK_THREAD)
        {
            msg_read = 0;
            printf("*------------------------------------------*\n");
        }
    }
    close(sock);
    exit(0);
}