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

int create_sck_sender()
{
    int socketSD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketSD <= 0)
    {
        printf("Error: Could not open socket.\n");
        return -1;
    }

    // set socket options enable broadcast
    int broadcastEnable = 1;
    int ret_create = setsockopt(socketSD, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret_create)
    {
        printf("Error: Could not open set socket to broadcast mode\n");
        close(socketSD);
        return -1;
    }
    //array_socks[c_sck++] = socketSD;
    return socketSD;
}

int main()
{
    /* int listeningSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
    int msg_read = 0; */
    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &broadcastAddr.sin_addr);
    broadcastAddr.sin_port = htons(8585);

    int socketSD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketSD <= 0)
    {
        printf("Error: Could not open socket.\n");
        return -1;
    }

    // set socket options enable broadcast
    int broadcastEnable = 1;
    int ret_create = setsockopt(socketSD, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret_create)
    {
        printf("Error: Could not open set socket to broadcast mode\n");
        close(socketSD);
        return -1;
    }
    //array_socks[c_sck++] = socketSD;
    socklen_t receiveSockaddrLen = sizeof(broadcastAddr);
    int listeningSocket = create_sck_sender();
    int msg_read = 0;
    while (1)
    {
        char *buf = malloc(BUFF_MSG);
        printf("hola\n");
        ssize_t result = recvfrom(listeningSocket, buf, BUFF_MSG, 0, (struct sockaddr *)&broadcastAddr, &receiveSockaddrLen);
        printf("hola\n");
        if (result > 0)
        {
            printf("--> %s\n", buf);
            ++msg_read;
        }
        else {
            printf("%ld\n", result);
        }
        free(buf);
        if (msg_read == NUM_SCK_THREAD)
        {
            msg_read = 0;
            printf("*------------------------------------------*\n");
        }
    }
}