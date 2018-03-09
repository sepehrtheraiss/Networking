#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define port 80 // default http port number
int main()
{
    char* host = "www.webscantest.com";
    struct hostent *server;
    struct sockaddr_in serv_addr;
/*
    int sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd < 0 )
    {
        perror("socket failed");
    }
*/
    // lookup server's ip address
    server = gethostbyname(host);
    if(server == NULL)
    {
        perror("error on gethostbyname");
        exit(1);
    }
    char** addrs = server->h_addr_list;
    for(int i =0;addrs[i]!=NULL;i++)
    {
        printf("%s\n",addrs[i]);
    }
    return 0;
} 
