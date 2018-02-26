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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    
//    dg_echo(sockfd,(struct sockaddr*)&servaddr,sizeof(cliaddr));
    int n;
    socklen_t cli_len,ser_len;
    cli_len = sizeof(cliaddr);
    ser_len = sizeof(servaddr);
    char recv_line[BUFF_SIZE+1];
    char send_line[BUFF_SIZE];
    char buff[BUFF_SIZE+1];
    uint32_t ports = 5000;
    char* str = NULL;
    int buff_read;
    char PATH[BUFF_SIZE];
    int fd;
    struct stat f_stat;
    uint32_t off_bytes[2];
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
    //cliaddr;
    for(;;)
    {
        n = recvfrom(sockfd,recv_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,&cli_len);
        recv_line[n]=0;
        // request for file size
        //printf("msg: %s\n",recv_line);
       // printf("port: %i\n",servaddr.sin_port);
        stat_code = parse(recv_line,n);
        printf("code: %i striped msg: %s\n",stat_code,recv_line);
        // send file size
        if(stat_code == 0) // if client gets a timeout fork will execute again
        {

            bzero(buff,BUFF_LEN);
            strcpy(buff,PATH);
            strcat(buff,"bin/");
            strcat(buff,recv_line);
            printf("path for file: %s\n",buff);
            stat(buff,&f_stat);
            sprintf(send_line,"{0:%lli}$",f_stat.st_size);
            sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
            printf("size file:%lli\n",f_stat.st_size);
            f_stat.st_size = 0;

        }
        else if(stat_code == 1)
        {
            //sprintf(send_line,"{9:got your file(%s) request}",recv_line);
            //sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
            printf("IP:%i PORT:%i\n",cliaddr.sin_addr.s_addr,cliaddr.sin_port );
            if(fork()==0)
            {
                sleep(1);
                int sockfd_new;
                struct sockaddr_in servaddr_new;
                sockfd_new = socket(AF_INET,SOCK_DGRAM,0);
                bzero(&servaddr_new,sizeof(servaddr_new));
                servaddr_new.sin_family = AF_INET;
                servaddr_new.sin_addr.s_addr = htonl(INADDR_ANY);
                servaddr_new.sin_port = 0;//htons(ports++);

                if(bind(sockfd_new,(struct sockaddr*)&servaddr_new,sizeof(servaddr_new)) < 0)
                {
                    perror("error on binding new sock");
                }
                //send_wait(char* msg,int sockfd,struct sockaddr* serv_addr,socklen_t servlen)
                FILE* file;
                char file_name[32];
                p_offset(recv_line,file_name,&off_bytes[0],&off_bytes[1]);
                bzero(buff,BUFF_LEN);
                strcpy(buff,PATH);
                strcat(buff,"bin/");
                strcat(buff,file_name);
                file = fopen(buff,"r");
                str = malloc(sizeof(char)*off_bytes[1]); // allocate array[bytes]
                int bytes_read = read_offset(file,off_bytes[0],off_bytes[1],str);
                int bytes_readCounter = bytes_read;
                int nDigits = 0;
                if(off_bytes[0] == 0)
                {
                    nDigits = 1;
                }
                else
                {
                    nDigits = floor(log10(off_bytes[0])) + 1;
                }
                int used_space = 5 + strlen(file_name) + nDigits + 3; // max bytes to read is 3 bytes
                int start = 0;
                int string_space = BUFF_SIZE - used_space;
                //printf("%i %i\n",used_space,string_space );
                printf("IP:%i PORT:%i\n",cliaddr.sin_addr.s_addr,cliaddr.sin_port );
                while(bytes_read > string_space)
                {
                    
                    memcpy(buff,str+start,string_space);
                    
                    buff[string_space] = 0;
                    //printf("%s %i %i %lu\n",file_name,off_bytes[0],string_space,strlen(buff));
                    snprintf(send_line,sizeof(send_line),"{2:%s:%i,%i:%s",file_name,off_bytes[0],string_space,buff);
                    //write(1,send_line,string_space);
                    if(sendto(sockfd_new,send_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,cli_len)<1)
                    {
                        perror("error on sendto filechunk");
                        exit(1);
                    }
                    //printf("here2\n");
                    start += string_space;
                    bytes_read -= string_space;
                }
               // printf("here3\n");
                memcpy(buff,str+start,bytes_read);
                buff[bytes_read] = 0;
                snprintf(send_line,sizeof(send_line),"{2:%s:%i,%i:%s",file_name,start,bytes_read,buff);//,bytes_read);
               // write(1,send_line,bytes_read);
                sendto(sockfd_new,send_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,cli_len);
                //printf("here4\n");
                free(str);

            }

        }
        else if(stat_code == -1)
        {
            sprintf(send_line,"{im up}");
            printf("%s\n",send_line);
            sendto(sockfd,send_line,BUFF_SIZE/4,0,(struct sockaddr*)&cliaddr,cli_len);
        }


    }

    return 0;
}
