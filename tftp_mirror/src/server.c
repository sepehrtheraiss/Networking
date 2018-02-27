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
int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr,"usage %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
     }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_LOOPBACK
    servaddr.sin_port = htons(atoi(argv[1]));
    //printf("my port %i\n",servaddr.sin_port);
    if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        perror("error on binding");
        exit(1);
    }
    //newSock(&sockfd,&servaddr,atoi(argv[1]),1);
    int n;
    socklen_t cli_len,ser_len;
    cli_len = sizeof(cliaddr);
    ser_len = sizeof(servaddr);
    char recv_line[BUFF_SIZE+1];
    char send_line[BUFF_SIZE];
    char buff[BUFF_SIZE+1];
    char* str = NULL;
    int buff_read;
    char PATH[BUFF_SIZE];
    int fd;
    struct stat f_stat;
    int off_bytes[2];
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
            if(fork()==0)
            {
                char fsend_line[BUFF_SIZE];
                char frecv_line[BUFF_SIZE];
                int foff_bytes[2];
                strcpy(frecv_line,recv_line);
                int sockfd_new;
                struct sockaddr_in servaddr_new;
                newSock(&sockfd,&servaddr_new,0,1);
                if (sockfd < 0) {
                    perror("ERROR opening socket");
                    exit(EXIT_FAILURE);
                }

                FILE* file;
                char file_name[32];
                p_offset(frecv_line,file_name,&foff_bytes[0],&foff_bytes[1]);
                bzero(buff,BUFF_LEN);
                strcpy(buff,PATH);
                strcat(buff,"bin/");
                strcat(buff,file_name);
                printf("filename: %s off:%i bytes:%i\n",file_name,foff_bytes[0],foff_bytes[1]);
                file = fopen(buff,"r");
                str = malloc(sizeof(char)*foff_bytes[1]); // allocate array[bytes]
                int bytes_read = read_offset(file,foff_bytes[0],foff_bytes[1],str);
                write(1,str,bytes_read);


                int bytes_readCounter = bytes_read;
                int nDigits = 0;
                if(foff_bytes[0] == 0)
                {
                    nDigits = 1;
                }
                else
                {
                    nDigits = floor(log10(foff_bytes[0])) + 1;
                }
                int used_space = 5 + strlen(file_name) + nDigits + 3; // max bytes to read is 3 bytes
                int start = 0;
                int string_space = BUFF_SIZE - used_space;
                //printf("%i %i\n",used_space,string_space );
                //printf("IP:%i PORT:%i\n",cliaddr.sin_addr.s_addr,cliaddr.sin_port );
                while(bytes_read > string_space)
                {
                    
                    memcpy(buff,str+start,string_space);
                    
                    buff[string_space] = 0;
                    //printf("%s %i %i %lu\n",file_name,off_bytes[0],string_space,strlen(buff));
                    snprintf(fsend_line,sizeof(fsend_line),"{2:%s:%i,%i:%s",file_name,foff_bytes[0],string_space,buff);
                  //  write(1,send_line,string_space);
                    if(sendto(sockfd_new,fsend_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,cli_len)<1)
                    {
                        perror("error on sendto filechunk");
                        exit(1);
                    }
                    //printf("here2\n");
                    start += string_space-1;
                    bytes_read -= string_space;
                }
               // printf("here3\n");
                memcpy(buff,str+start,bytes_read);
                buff[bytes_read] = 0;
                snprintf(fsend_line,sizeof(fsend_line),"{2:%s:%i,%i:%s",file_name,start,bytes_read,buff);//,bytes_read);
               // write(1,send_line,bytes_read);
                sendto(sockfd_new,fsend_line,BUFF_SIZE,0,(struct sockaddr*)&cliaddr,cli_len);
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
