#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#define BUFF_LEN 512
#define SERV_PORT 69

void dg_cli(FILE* fp,int sockfd,struct sockaddr* serv_addr,socklen_t servlen)
{
    int n;
    char sendline[BUFF_LEN],recvline[BUFF_LEN];
    socklen_t len;
    struct sockaddr* reply_addr = malloc(servlen);

    //sendto(sockfd,"bitches",strlen(sendline),0,sa,servlen);
    while(fgets(sendline,BUFF_LEN,fp)!=NULL)
    {
        sendto(sockfd,sendline,strlen(sendline),0,serv_addr,servlen);
        len = servlen;
        n = recvfrom(sockfd,recvline,BUFF_LEN,0,reply_addr,&len);
        if(len != servlen || memcmp(serv_addr,reply_addr,len) != 0)
        {
            printf("reply from %p (ignore)\n",reply_addr);
        }
        recvline[n] = 0;
        fputs(recvline,stdout);
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
