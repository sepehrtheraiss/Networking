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
#include "../include/queue.h"
#define BUFF_LEN 512

int main(int argc,char** argv)
{   
    int sockfd;
    struct sockaddr_in servaddr;
    if(argc < 4)
    {
        fprintf(stderr,"usage: %s server-info.txt num-chunks filename\n", argv[0]);
        exit(1);
    }
    FILE* s_file = fopen(argv[1],"r");
    if(s_file == NULL){perror("error fopen on argv[1]");exit(1);}
    server servers[10];
    up = 0;
    file_size = 0;
    int num_servs = server_info(s_file,servers); // number of servers in the server-info
    fclose(s_file);


    for(int i =0;i < num_servs;i++)
    {
        servers[i].id = -1;
    }
    // checks which servers are up then assigns id to each one
    // need to find max number of servers up
    for(int i=0; i < num_servs;i++)
    {
        if(isUp(&servers[i]) == 1)
        {
            //servers[i].q[0] = q_init(filename);
            for(int j=0;j<16;j++)
            {
                servers[i].q[j] = NULL; 
            } 
            servers[i].counter = 0;
            servers[i].nextQ = 0;
            servers[i].id = up++;
        }
    }
    printf("servs up:%i\n", up);
    if(up == 0)
    {
        printf("no servers are up try again later\n");
        exit(1);
    }
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in tempserv_addr;
    char file[BUFF_LEN];
    strcpy(filename,argv[3]); 
    sprintf(file,"{0:%s}$",argv[3]); // get file size
    int stop = 0;
    for(int i=0; i < num_servs && stop != 1;i++)
    {
        if(servers[i].id != -1)
        {
            //make connection
            bzero((char *) &tempserv_addr, sizeof(tempserv_addr));
            tempserv_addr.sin_family = AF_INET;
            tempserv_addr.sin_addr.s_addr = servers[i].IP;
            tempserv_addr.sin_port = servers[i].port;
            file_size = getFileSize(file,sockfd,(struct sockaddr*)&tempserv_addr,sizeof(tempserv_addr));
            stop = 1;
        }
    }
    if(file_size == 0)
    {
        printf("no such file exist\nexiting now\n");
        exit(1);
    }
    printf("file size: %i\n",file_size);
    SUP = up ;// serves up this cannot be modified
    //uint32_t chuncks = atoi(argv[2]);
    chunck = atoi(argv[2]);
    pthread_t thread[chunck];
    uint8_t i = 0;
    uint32_t offset = 0;
    uint8_t place_holder = 0 ;
    uint8_t num_chuncks = chunck;
    uint8_t download_failed = 0;
    uint16_t FRAG_SIZE = ceil((double)file_size /chunck);
    while(complete != 1 && download_failed != 1)
    {
        download_failed++;
        while(num_chuncks != 0)
        {
            if(servers[i%num_servs].id != -1)
            {
                place_holder = servers[i%num_servs].nextQ++;
                if(place_holder > 16)
                {
                    fprintf(stderr, "counter over flow\n");
                    exit(1);
                }
                printf("nextQ:%i\n",place_holder);
                servers[i%num_servs].q[place_holder] = q_init(filename,offset,FRAG_SIZE); 
                pthread_create(&thread[num_chuncks-1],NULL,(void *)initThread,(server*)&servers[i%num_servs]);
                offset += FRAG_SIZE; 
                num_chuncks--;
            }
            i++;
        }
        while(up != 0); // busy waiting
        up = SUP;
        num_chuncks = chunck;
        printf("re-try\n");
    }
   
    return 0;
}
