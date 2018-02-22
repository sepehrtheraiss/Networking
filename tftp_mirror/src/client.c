#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "../include/header.h"
#define BUFF_LEN 512
#define SERV_PORT 4200

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
    char file[BUFF_LEN];
    sprintf(file,"{1:%s}","test_file");
    int d_port = getFileSize_port(file,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));// returns the port to request the file
    printf("data port: %i\n",d_port);
//    servaddr.sin_port = htons(d_port);
 //   bindNewPort();
//    getFile();
//    dg_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    return 0;
}
