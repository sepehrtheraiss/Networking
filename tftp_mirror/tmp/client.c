#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#define BUFF_LEN 512
#define SERV_PORT 4200


void 
dg_cli(FILE* fp,int sockfd,struct sockaddr* serv_addr,socklen_t servlen)
{
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set readfds;

    int n;
    char sendline[BUFF_LEN],recvline[BUFF_LEN];
    socklen_t len;
    struct sockaddr* reply_addr = malloc(servlen);
    FD_ZERO(&readfds);
    while(fgets(sendline,BUFF_LEN,fp)!=NULL)
    {
        FD_SET(sockfd,&readfds);
        sendto(sockfd,sendline,strlen(sendline),0,serv_addr,servlen);
        len = servlen;
        select(sockfd+1,&readfds,NULL,NULL,&timeout);
        if(FD_ISSET(sockfd,&readfds))
        {
            n = recvfrom(sockfd,recvline,BUFF_LEN,0,reply_addr,&len);
            if(len != servlen || memcmp(serv_addr,reply_addr,len) != 0)
            {
                printf("reply from %p (ignore)\n",reply_addr);
            }
            recvline[n] = 0;
            fputs(recvline,stdout);
        }
        else
        {
            printf("timeout\n");
        }
    }
}
int main(int argc,char** argv)
{   
    int sockfd;
    struct sockaddr_in servaddr;
    if(argc < 2)
    {
        fprintf(stderr,"usage: IP\n");
        exit(1);
    }
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    dg_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    return 0;
}
