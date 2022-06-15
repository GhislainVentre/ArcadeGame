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

#include "customSTD.h"

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

    char tabuleiro[3][3];
    int iterator;
    int linhaJogada, colunaJogada;
    int posicaoJogada;
    int rodada = 0;
    int gameStatus = 1;
    int valid_play = 0;
    int played;
    int numberPlayed;
    int posicoes[9][2] = {{2, 0}, {2, 1}, {2, 2}, {1, 0}, {1, 1}, {1, 2}, {0, 0}, {0, 1}, {0, 2}};

    char errorMessage[255] = {'\x00'};
    char *nomeJogadorAtual;
    char message[BUFFER_SZ] = {};

    int receive = recv(sockfd, message, BUFFER_SZ, 0);

    if (receive > 0) {
        setbuf(stdin, 0);
        trim_lf(message, strlen(message));
        sscanf(message, "%s", &namePlayer2[0]);

        setbuf(stdout, 0);
        setbuf(stdin, 0);

        bzero(message, BUFFER_SZ);

        rodada = 0;

        while (rodada < 9 && gameStatus == 1)
        {
            if (playerStatus == 1)
            {
                nomeJogadorAtual = (char *)&namePlayer1;
            } 
            else
            {
                nomeJogadorAtual = (char *)&namePlayer2;
            }

            printf("\nRodada: %d", rodada);
            printf("\nJogador: %s\n", nomeJogadorAtual);

            while (valid_play == 0)
            {
                bzero(message, BUFFER_SZ);

                int receive = recv(sockfd, message, BUFFER_SZ, 0);

                if (receive > 0) {
                    valid_play = 1;

                    setbuf(stdin, 0);
                    setbuf(stdout, 0);

                    if (strcmp(message, "vez1\n") == 0)
                    {
                        printf("Digite uma posicao: ");
                        scanf("%d", &posicaoJogada);


                        linhaJogada = posicoes[posicaoJogada - 1][0];
                        colunaJogada = posicoes[posicaoJogada - 1][1];


                        if (valid_play == 1)
                        {
                            sprintf(message, "play %i\n", posicaoJogada);
                            send(sockfd, message, strlen(message), 0);
                            bzero(message, BUFFER_SZ);
                        }
                        
                    }
                    else if (strcmp(message, "vez2\n") == 0)
                    {
                        printf("O outro jogador esta jogando...\n");

                        played = 0;

                        while (played == 0)
                        {
                            int receive = recv(sockfd, message, BUFFER_SZ, 0);

                            if (receive > 0) {
                                sscanf(message, "%i", &numberPlayed);

                                linhaJogada = posicoes[numberPlayed - 1][0];
                                colunaJogada = posicoes[numberPlayed - 1][1];

                                played = 1;
                            }
                        }

                        valid_play = 1;
                    }
                }
                else
                {
                    valid_play = 0;
                }
            }

            if (playerStatus == 1)
            {
                tabuleiro[linhaJogada][colunaJogada] = 'X';
                playerStatus = 2;
            } 
            else
            {
                tabuleiro[linhaJogada][colunaJogada] = 'O';
                playerStatus = 1;
            }

            for (iterator = 0; iterator < 3; iterator++)
            {
                if (
                    (
                        (tabuleiro[iterator][0] == tabuleiro[iterator][1]) && (tabuleiro[iterator][1] == tabuleiro[iterator][2]) && tabuleiro[iterator][0] != '-'
                    )
                        ||
                    (
                        (tabuleiro[0][iterator] == tabuleiro[1][iterator]) && (tabuleiro[1][iterator] == tabuleiro[2][iterator]) && tabuleiro[0][iterator] != '-'
                    )
                )
                {
                    gameStatus = 0;
                }
            }

            if (
                (
                    (tabuleiro[0][0] == tabuleiro[1][1]) && (tabuleiro[1][1] == tabuleiro[2][2]) && tabuleiro[0][0] != '-'
                )
                    ||
                (
                    (tabuleiro[0][2] == tabuleiro[1][1]) && (tabuleiro[1][1] == tabuleiro[2][0]) && tabuleiro[0][2] != '-'
                )
            )
            {
                gameStatus = 0;
            }

            rodada++;
            valid_play = 0;
            bzero(message, BUFFER_SZ);
        }

        bzero(message, BUFFER_SZ);

        int receive = recv(sockfd, message, BUFFER_SZ, 0);

        if (receive > 0) {
            setbuf(stdin, 0);
            setbuf(stdout, 0);

            if (strcmp(message, "win1\n") == 0)
            {
                printf("\nO jogador '%s' venceu!", nomeJogadorAtual);
            }
            else if (strcmp(message, "win2\n") == 0)
            {
                printf("\nO jogador '%s' venceu!", nomeJogadorAtual);
            }

            printf("\nFim de jogo!\n");

            sleep(6);

            if (pthread_create(&lobby_thread, NULL, &lobby, NULL) != 0) {
                printf("ERROR: pthread\n");
                return NULL;
            }

            if (pthread_create(&recv_msg_thread, NULL, (void*)recv_msg_handler, NULL) != 0) {
                printf("ERROR: pthread\n");
                return NULL;
            }

            pthread_detach(pthread_self());
            pthread_cancel(multiplayer_game);

        }
        
    }

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
                printf("Comandos:\n");
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
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    // send the name
    send(sockfd, name, NAME_LEN, 0);

    if (pthread_create(&lobby_thread, NULL, &lobby, NULL) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&recv_msg_thread, NULL, (void*)recv_msg_handler, NULL) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    while(1)
    {
        if (flag) {
            printf("\nBye\n");
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

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "wlp0s20f3", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* return result */
    char* ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

    return ip;
}

int main(int argc, char **argv)
{
    char *ip = getIPv4();
    int port = atoi(argv[2]);

    connectGame(&ip, port);

    return 0;
}
