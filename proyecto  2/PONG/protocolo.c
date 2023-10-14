#include "glo.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "proto.h"

int done = FALSE;
int sockfd;

pthread_mutex_t mutexsum = PTHREAD_MUTEX_INITIALIZER;

char send_buffer[BUF_SIZE];
char recv_buffer[BUF_SIZE + USERNAME_LEN];
char username[USERNAME_LEN];

void start_server(pthread_t threads[2], pthread_attr_t attr){
    bzero(send_buffer, BUF_SIZE);
    bzero(recv_buffer, BUF_SIZE + USERNAME_LEN);

    int portnum = PORT;

    printf("Digite su nombre de usuario: ");
    fgets(username, USERNAME_LEN, stdin);
    username[strlen(username) - 1] = '\0';

    char ip[16];
    
    strcpy(ip, IP);
    printf("IP Address: %s\n", ip);

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == SYSERR)
    {
        close(sockfd);
        perror("ERROR: CREATING SOCKET");
        exit(EXIT_FAILURE);
    }

    struct hostent *server_host;
    server_host = gethostbyname(IP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    server_addr.sin_addr = *((struct in_addr *)server_host->h_addr);

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SYSERR){
        perror("CONNECTION WITH SERVER: FAILED");
        exit(EXIT_FAILURE);
    }

    printf("CONNECTION WITH SERVER: SUCCESSFULL\n");

    strcpy(send_buffer, username);
    if(send(sockfd, send_buffer, strlen(send_buffer), 0) == SYSERR){
        perror("ERROR: SENDING USERNAME TO SERVER");
        exit(EXIT_FAILURE);
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

void *sender()
{
    while (1)
    {
        bzero(send_buffer, BUF_SIZE);
        fgets(send_buffer, BUF_SIZE, stdin);

        if (send(sockfd, send_buffer, strlen(send_buffer), 0) == SYSERR)
        {
            perror("send");
            done = TRUE;
            pthread_mutex_destroy(&mutexsum);
            pthread_exit(NULL);
        }

        if (strcmp(send_buffer, CLOSE) == 0 || strcmp(send_buffer, EXIT) == 0)
        {
            done = TRUE;
            pthread_mutex_destroy(&mutexsum);
            pthread_exit(NULL);
        }

        pthread_mutex_unlock(&mutexsum);
    }
}

void *receiver()
{
    int nbytes;

    while (1)
    {
        bzero(recv_buffer, BUF_SIZE);
        if ((nbytes = recv(sockfd, recv_buffer, BUF_SIZE - 1, 0)) == SYSERR)
        {
            perror("recv");
            done = TRUE;
            pthread_mutex_destroy(&mutexsum);
            pthread_exit(NULL);
        }

        recv_buffer[nbytes] = '\0';
        if (strcmp(ERROR, recv_buffer) == 0)
        {
            printf("ERROR: USERNAME %s ALREADY TAKEN.\n", send_buffer);
            done = TRUE;
            pthread_mutex_destroy(&mutexsum);
            pthread_exit(NULL);
        }
        else
        {
            printf("%s", recv_buffer);
            pthread_mutex_unlock(&mutexsum);
        }
    }
}