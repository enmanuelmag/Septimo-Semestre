#include <time.h>
#include <stdio.h>
#include <string.h>
#define __USE_GNU
#include <sched.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFF_MSG 999999
#define NUM_SCK_THREAD 7

sem_t sem_msg[NUM_SCK_THREAD];

int main()
{
    int listeningSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listeningSocket <= 0)
    {
        printf("Error: listenForPackets - socket() failed.\n");
        //closeall
        return -1;
    }

    // set timeout to 2 seconds.
    struct timeval timeV;
    timeV.tv_sec = 0;
    timeV.tv_usec = 500;

    if (setsockopt(listeningSocket, SOL_SOCKET, SO_RCVTIMEO, &timeV, sizeof(timeV)) == -1)
    {
        printf("Error: listenForPackets - setsockopt failed\n");
        close(listeningSocket);
        //closeall
        return -1;
    }

    // bind the port
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8585);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int status = bind(listeningSocket, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (status == -1)
    {
        close(listeningSocket);
        printf("Error: listenForPackets - bind() failed. Dash\n");
        //closeall
        return -1;
    }

    // receive
    struct sockaddr_in receiveSockaddr;
    socklen_t receiveSockaddrLen = sizeof(receiveSockaddr);
    int msg_read = 0;
    while (1)
    {
        for (int i = 0; i < NUM_SCK_THREAD; i++)
        {
            char *buf = malloc(BUFF_MSG);
            ssize_t result = recvfrom(listeningSocket, buf, BUFF_MSG, 0, (struct sockaddr *)&receiveSockaddr, &receiveSockaddrLen);
            if (result > 0)
            {
                printf("--> %s\n", buf);
                ++msg_read;
            }
            free(buf);
        }
        if (msg_read == NUM_SCK_THREAD)
        {
            msg_read = 0;
            printf("*------------------------------------------*\n");
        }
    }
}