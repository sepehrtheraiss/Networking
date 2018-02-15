#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>
#include "../include/header.h"
int main(int argc, char** argv) {
    /*usage: server-info.txt num-connections filename*/
    if (argc < 4) {
        fprintf(stderr,"usage %s server-info.txt num-connections filename\n", argv[0]);
        exit(0);
    }
    server servers[10];
    up = 0;
    pthread_cond_init(&lock,NULL);
    //lock = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&m,NULL);
    //m = PTHREAD_MUTEX_INITIALIZER;
    //char buffer[BUFF_SIZE];
    //unsigned int fileSize; global
    //int servs_req; global
    int nc = atoi(argv[2]); // pass in server nums by the user
   // char filename[32];
    strcpy(filename,argv[3]);
    FILE* s_file = fopen(argv[1],"r");
    if(s_file == NULL){perror("error fopen on argv[1]");exit(1);}
    int num_servs = server_info(s_file,servers); // number of servers in the server-info
    // global t_servers
    fclose(s_file);
    // number of connections can't be more than the number of servers in the server-info.txt
    if(nc > num_servs)
    {
        nc = num_servs;
    }
    // gets the file size from the first responsive server
    if((fileSize = getFileSize(servers,filename,num_servs,nc)) < 1)
    {
        perror("file either does not exist or all the servers are down");
        exit(1);
    }
    // init serves id = -1
    for(int i =0;i < num_servs;i++)
    {
        servers[i].id = -1;
    }
    // checks which servers are up then assigns id to each one
    // need to find max number of servers up
    for(int i=0; i < num_servs && up < nc;i++)
    {
        if(isUp(&servers[i]) == 1)
        {
            servers[i].str_arr = malloc(sizeof(char*)*10);
            servers[i].id = up++;
        }
    }

    pthread_t thread[up];
    for(int i =0; i < num_servs;i++)
    {
        if(servers[i].id != -1)
        {
            pthread_create(&thread[i],NULL,(void *)initThread,(server*)&servers[i]); 
        }
    }
    while(up != 0);// busy waiting until all servs have done their job
    for(int i =0; i < num_servs;i++)
    {
        if(servers[i].id != -1)
        {
            for(int j=0;servers[i].str_arr[j]!=NULL && j < 10;j++)
            {
                printf("%s",servers[i].str_arr[j]);
                free(servers[i].str_arr[j]);
            }
            free(servers[i].str_arr);
        }
    }
    
    /*
    pthread_t thread[nc];
    for(int i =0;i < num_servs;i++) // go thru much servers as possible
    {
        servers[i].id = i;
        servers[i].str = NULL;
        pthread_create(&thread[i],NULL,(void *)initThread,(server*)&servers[i]); 
       // pthread_join(thread[i],NULL);
    }
    // can go negative for the extra undeeded connections
    while(nc > 0);
    for(int i =0;i< num_servs;i++)
    {
        pthread_join(thread[i],NULL);
    }
    //pthread_cond_broadcast(&lock);
    for(int i =0;i < num_servs;i++)
    {
       if(servers[i].str != NULL)
       {
            printf("%s",servers[i].str);
       }
    }*/
   /* 
    // seek and read
    // fragment size, need to take the ceiling if frag size has a floating number
    const unsigned int FRAG_SIZE = ceil((double)fileSize / nc); // ?? at compile time
    char buffer[BUFF_SIZE];
    //printf("filesize: %i fragment: %i \n",fileSize,FRAG_SIZE);
    int j=0;
    for(int i =0;i<nc;i++)
    {
        sendOffsetRead(&servers[i],j,FRAG_SIZE,buffer,&filename[0]);
        j += FRAG_SIZE;
    }
    */
    return 0;
}
