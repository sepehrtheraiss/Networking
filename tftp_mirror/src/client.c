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
    //int chuncks = atoi(argv[2]);
    chunck = atoi(argv[2]);
    pthread_t thread[chunck];
    int i = 0;
    int offset = 0;
    int nextQ = 0 ;
    int num_chuncks = chunck;
    int download_failed = 0;
    int FRAG_SIZE = ceil((double)file_size /chunck);
    while(complete != 1 && download_failed != 1)
    {
        download_failed++;
        while(num_chuncks != 0)
        {
            if(servers[i%num_servs].id != -1)
            {
                nextQ = servers[i%num_servs].nextQ++;
                if(nextQ > 16)
                {
                    fprintf(stderr, "counter over flow\n");
                    exit(1);
                }
                printf("nextQ:%i\n",nextQ);
                servers[i%num_servs].q[nextQ] = q_init(filename,offset,FRAG_SIZE); 
                //initThread(&servers[i%num_servs]);
                pthread_create(&thread[num_chuncks-1],NULL,(void *)initThread,(server*)&servers[i%num_servs]);
                offset += FRAG_SIZE; 
                num_chuncks--;
            }
            i++;
        }
        for(int j =0;j<chunck;j++)
        {
            pthread_join(thread[j],NULL);
        }
        //while(up != 0); // busy waiting
        up = SUP;
        num_chuncks = chunck;
        printf("re-try\n");
    }
    // arrage the queues from min offset to max
    queue** q_arr[chunck];//q_init(NULL,0,0);
    int q_index = 0;
    int next_bytes = 0;
    while(next_bytes < file_size)
    {
        for(int j =0;j < chunck;j++)
        {
            if(servers[j].id != -1)
            {
                for(int v = 0; v <servers[j].nextQ;v++)
                {
                    if(servers[j].q[v]->off_bytes[0] == next_bytes)
                    {
                        q_arr[q_index++] = &servers[j].q[v];
                        next_bytes += servers[j].q[v]->off_bytes[1];
                        //printf("next_bytes: %i\n", next_bytes);
                    }
                }
            }
        }
    }

    char PATH[BUFF_SIZE];
    FILE* pwd = popen("pwd","r");
    int buff_read = fread(PATH,1,BUFF_SIZE,pwd);
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
    char f_file[BUFF_LEN];
    strcpy(f_file,PATH);
    strcat(f_file,"bin/");
    strcat(f_file,filename);
    creat(f_file, S_IRUSR+S_IWUSR);
    FILE* nf = fopen(f_file,"a");
    for(int v = 0; v <chunck;v++)
    {
        printQ(*q_arr[v],nf);
        q_deinit(*q_arr[v]);
    }
    fclose(nf);
    /*
    for(int j =0;j < chunck;j++)
    {
        if(servers[j%num_servs].id != -1)
        {
            for(int v = 0; v <servers[j%num_servs].nextQ;v++)
            {
                printQ(servers[j%num_servs].q[v],stdout);
            }
        }
    }
    */
    return 0;
}
