#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// include network stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>

struct client {
    int nb;
    char ip[20];
    int port;
    char pseudo[30];
} client[10];

int nbClients;
int server_socket;
int server_ip;

int main(int argc, char *argv[]) {
    
    // create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Error creating socket\n");
        exit(1);
    }
    printf("Socket created: %d\n", server_socket);

    //find my ip
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(1);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) { // loop through all interfaces
        if (ifa->ifa_addr == NULL) continue; // skip if no address
        if (strcmp(ifa->ifa_name, "wlp2s0") == 0) { // check if interface is wlp2s0
            server_ip = inet_addr(inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr)); // get ip
            printf("IP: %s\n", inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr)); // prints ipv4 address
        }
    }
    freeifaddrs(ifaddr);

    // TODO : find a way to only get the good ip

    return 0;
}