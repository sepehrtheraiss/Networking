#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#define BUFF_LEN 512
#define SERV_PORT 4200

void dg_echo(int sockfd, struct sockaddr* serv_addr,socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[BUFF_LEN];

    for(;;)
    {
        len = clilen;
        n = recvfrom(sockfd,msg,BUFF_LEN,0,serv_addr,&len);
        printf("msg: %s\n",msg);
        sendto(sockfd,msg,n,0,serv_addr,len);
    }
}
int main()
{
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port = htons(SERV_PORT);

    bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

    dg_echo(sockfd,(struct sockaddr*)&servaddr,sizeof(cliaddr));
    return 0;
}
