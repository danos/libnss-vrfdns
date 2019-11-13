#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int i;
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname("google.com")) == NULL) {  // get the host info
        herror("gethostbyname");
        return 2;
    }

    // print information about this host:
    printf("Official name is: %s\n", he->h_name);
    printf("    IP addresses: ");
    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");

    struct hostent *he2;
    struct in_addr ipv4addr;
    //reverse
    inet_pton(AF_INET, inet_ntoa(*addr_list[0]), &ipv4addr);
    he2 = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    printf("Host name: %s  -- %s\n", he2->h_name, inet_ntoa(*addr_list[0]));
    return 0;
}
