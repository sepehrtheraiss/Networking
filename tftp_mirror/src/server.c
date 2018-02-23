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
    char buff[BUFF_SIZE];
    int buff_read;
    unsigned int d_port = 5000; // data port will increment on each call
    FILE* file;
    char PATH[BUFF_SIZE];
    int fd;
    struct stat f_stat;
    FILE* pwd = popen("pwd","r");
    buff_read = fread(PATH,1,BUFF_SIZE,pwd);
    pclose(pwd);
    if(buff_read+1 > BUFF_SIZE){ // +1 for null
        perror("buff read > buff size");
        exit(1);
    }
    // appending / to the path
    if((int)PATH[buff_read-1] == 10) // new line feed
    {
        PATH[buff_read-1] = '/';
        PATH[buff_read] = 0;
    }
    else
    {
        PATH[buff_read] = '/';
        PATH[buff_read+1] = 0;
    }
    int stat_code = 0;
    for(;;)
    {

        n = recvfrom(sockfd,recv_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,&cli_len);
        // request for file size
        //printf("msg: %s\n",recv_line);
       // printf("port: %i\n",servaddr.sin_port);
        stat_code = parse(recv_line,n);
        printf("msg:%s\n",recv_line);
        if(stat_code == 1) // if client gets a timeout fork will execute again
        {

            bzero(buff,BUFF_LEN);
            strcpy(buff,PATH);
            strcat(buff,"bin/");
            strcat(buff,recv_line);
            printf("recv: %s\n",buff);
            //fd = open(recv_line,O_RDONLY);
            stat(buff,&f_stat);
            sprintf(send_line,"{1:%lli}$",f_stat.st_size);
            sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
            printf("size file:%lli\n",f_stat.st_size);

            //flcose(file);
/*            // fork() then bind new port then the port wait to listen then send data
            if(fork() == 0)
            {
                // data port
                int sockfd_data = socket(AF_INET,SOCK_DGRAM,0);
                struct sockaddr_in servaddr_data;
                newPort(&d_port,sockfd_data,&servaddr_data);
                //printf("new port: %i\n",d_port);
                sprintf(send_line,"{2:3231},{3:%i}",d_port);
                sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
                
            }
            */
        }
        else if(stat_code == -1)
        {
            sprintf(send_line,"{im up}");
            printf("%s\n",send_line );
            sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
        }


    }

    return 0;
}
