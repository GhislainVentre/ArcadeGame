#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

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

int main()
{
    char* ip = getIPv4();
    printf("%s\n", ip);

    return 0;
}