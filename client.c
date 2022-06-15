#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <pthread.h>
#include <sys/types.h>

#include "headers/customSTD.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048
#define NAME_LEN 32

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LEN];
int player = 1;

void *lobby(void *arg);

pthread_t lobby_thread;
pthread_t recv_msg_thread;
pthread_t multiplayer_game;

void catch_ctrl_c_and_exit()
{
    flag = 1;
}

void recv_msg_handler();

void *multiplayerGame(void *arg)
{
    int playerStatus = player;
    char namePlayer1[32];
    char namePlayer2[32];

    strcpy(namePlayer1, name);

    char errorMessage[255] = {'\x00'};
    char *nomeJogadorAtual;
    char message[BUFFER_SZ] = {};

    int receive = recv(sockfd, message, BUFFER_SZ, 0);

    return NULL;
}

void *lobby(void *arg)
{
    char buffer[BUFFER_SZ] = {};

    while(1)
    {
        str_overwrite_stdout();
        fgets(buffer, BUFFER_SZ, stdin);
        trim_lf(buffer, BUFFER_SZ);

        if (strcmp(buffer, "exit") == 0) {
            break;
        } else {
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(buffer, BUFFER_SZ);
    }

    catch_ctrl_c_and_exit(2);

    return NULL;
}

void recv_msg_handler()
{
    char message[BUFFER_SZ] = {};

    flashScreen();

    while(1)
    {
        int receive = recv(sockfd, message, BUFFER_SZ, 0);

        if (receive > 0) {
            if (strcmp(message, "ok") == 0)
            {
                printf("Commands:\n");
                printf("\t -list\t\t\t  List all tic-tac-toe rooms\n");
                printf("\t -create\t\t  Create one tic-tac-toe room\n");
                printf("\t -join {room number}\t  Join in one tic-tac-toe room\n");
                printf("\t -leave\t\t\t  Back of the one tic-tac-toe room\n");
                printf("\t -start\t\t\t  Starts one tic-tac-toe game\n\n");

                str_overwrite_stdout();
            }
            else if (strcmp(message, "start game\n") == 0)
            {
                pthread_cancel(lobby_thread);
                // pthread_kill(recv_msg_thread, SIGUSR1);   
                
                player = 1;
                if (pthread_create(&multiplayer_game, NULL, (void*)multiplayerGame, NULL) != 0) {
                    printf("ERROR: pthread\n");
                    exit(EXIT_FAILURE);
                }
                pthread_detach(pthread_self());
                pthread_cancel(recv_msg_thread);

                // pthread_kill(lobby_thread, SIGUSR1);   
            }
            else if (strcmp(message, "start game2\n") == 0)
            {
                pthread_cancel(lobby_thread);
                // pthread_kill(recv_msg_thread, SIGUSR1);   
                
                player = 2;
                if (pthread_create(&multiplayer_game, NULL, (void*)multiplayerGame, NULL) != 0) {
                    printf("ERROR: pthread\n");
                    exit(EXIT_FAILURE);
                }
                pthread_detach(pthread_self());
                pthread_cancel(recv_msg_thread);

                // pthread_kill(lobby_thread, SIGUSR1);   
            }
            else
            {
                printf("%s", message);
                str_overwrite_stdout();
            }
        } else if (receive == 0) {
            break;
        }

        bzero(message, BUFFER_SZ);
    }
}

void send_msg_handler()
{
    char buffer[BUFFER_SZ] = {};

    while(1)
    {
        str_overwrite_stdout();
        fgets(buffer, BUFFER_SZ, stdin);
        trim_lf(buffer, BUFFER_SZ);

        if (strcmp(buffer, "exit") == 0) {
            break;
        } else {
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(buffer, BUFFER_SZ);
    }

    catch_ctrl_c_and_exit(2);
}

int connectGame(char* ip, int port)
{
    setbuf(stdin, 0);

    printf("Enter your name: ");
    fgets(name, BUFFER_SZ, stdin);
    trim_lf(name, BUFFER_SZ);

    // strcpy(name, "murilo");

    if (strlen(name) > NAME_LEN - 1 || strlen(name) < 2) {
        printf("Enter name corretly\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    
    //socket settings
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // connect to the server
    int err = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect.\n");
        return EXIT_FAILURE;
    }

    // send the name
    send(sockfd, name, NAME_LEN, 0);

    if (pthread_create(&lobby_thread, NULL, &lobby, NULL) != 0) {
        printf("ERROR: pthread.\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&recv_msg_thread, NULL, (void*)recv_msg_handler, NULL) != 0) {
        printf("ERROR: pthread.\n");
        return EXIT_FAILURE;
    }

    while(1)
    {
        if (flag) {
            printf("\nBye!\n");
            break;
        }
    }

    close(sockfd);

    return 0;
}

char* getIPv4()
{
    int fd;
    struct ifreq ifr;
    struct if_nameindex *if_nidxs, *intf;
    if_nidxs = if_nameindex();

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    if(if_nidxs != NULL)
    {
        for (intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++)
        {
            /* I want IP address attached to "wlp0s20f3" */
            strncpy(ifr.ifr_name, intf->if_name, IFNAMSIZ-1);
        }
        if_freenameindex(if_nidxs);
    }
    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* return result */
    char* ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

    return ip;       
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *ip = getIPv4();
    int port = atoi(argv[1]);

    connectGame(ip, port);

    return 0;
}