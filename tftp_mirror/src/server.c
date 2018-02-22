#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "../include/header.h"
#define BUFF_LEN 512
#define SERV_PORT 4200
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
    
//    dg_echo(sockfd,(struct sockaddr*)&servaddr,sizeof(cliaddr));
    int n;
    socklen_t cli_len,ser_len;
    cli_len = sizeof(cliaddr);
    ser_len = sizeof(servaddr);
    char recv_line[BUFF_SIZE];
    char send_line[BUFF_SIZE];
    unsigned int d_port = 5000; // data port will increment on each call
    for(;;)
    {
        //len = clilen;
        n = recvfrom(sockfd,recv_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,&cli_len);
        // request for file size
        printf("msg: %s\n",recv_line);
        printf("port: %i\n",servaddr.sin_port);
        if(parse(recv_line,n) == 1) // if client gets a timeout fork will execute again
        {
            //sprintf(send_line,"{2:3231},{3:%i}",servaddr.sin_port);
            //sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
            // fork() then bind new port then the port wait to listen then send data
            if(fork() == 0)
            {
                // data port
                int sockfd_data = socket(AF_INET,SOCK_DGRAM,0);
                struct sockaddr_in servaddr_data;
                newPort(&d_port,sockfd_data,&servaddr_data);
               // socklen_t serdata_len;
               // getsockname(sockfd_data,(struct sockaddr*)&servaddr_data,&serdata_len);
                printf("new port: %i\n",d_port);
                sprintf(send_line,"{2:3231},{3:%i}",d_port);
                sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
                
            }
        }

        //printf("msg: %s\n",msg);
        //strcpy(msg,"{2:42567},{3:6969}");
        //sendto(sockfd,msg,BUFF_SIZE/4,0,(struct sockaddr*)&servaddr,len);
    }

    return 0;
}
