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
    sup = 0;
    pthread_cond_init(&lock,NULL);
    //lock = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&m,NULL);
    //m = PTHREAD_MUTEX_INITIALIZER;
    //char buffer[BUFF_SIZE];
    //unsigned int fileSize; global
    //int servs_req; global
    servs_req = atoi(argv[2]); // pass in server nums by the user
   // char filename[32];
    strcpy(filename,argv[3]);
    FILE* s_file = fopen(argv[1],"r");
    if(s_file == NULL){perror("error fopen on argv[1]");exit(1);}
    int num_servs = server_info(s_file,servers); // number of servers in the server-info
    // global t_servers
    t_servers = num_servs;
    fclose(s_file);
    // gets the file size from the first responsive server
    if((fileSize = getFileSize(servers,filename,num_servs,servs_req)) < 1)
    {
        perror("file either does not exist or all the servers are down");
        exit(1);
    }
    pthread_t thread[servs_req];
    for(int i =0;i < num_servs;i++) // go thru much servers as possible
    {
        servers[i].id = i;
        servers[i].str = NULL;
        pthread_create(&thread[i],NULL,(void *)initThread,(server*)&servers[i]); 
       // pthread_join(thread[i],NULL);
    }
    while(servs_req != 0);
    for(int i =0;i< num_servs;i++)
    {
        pthread_join(thread[i],NULL);
    }
    //pthread_cond_broadcast(&lock);
    for(int i =0;i< num_servs;i++)
    {
       if(servers[i].str != NULL)
       {
            printf("%s",servers[i].str);
       }
    }
    /*
    // seek and read
    // fragment size, need to take the ceiling if frag size has a floating number
    const unsigned int FRAG_SIZE = ceil((double)fileSize / servs_req); // ?? at compile time
    char buffer[BUFF_SIZE];
    //printf("filesize: %i fragment: %i \n",fileSize,FRAG_SIZE);
    int j=0;
    for(int i =0;i<servs_req;i++)
    {
        sendOffsetRead(&servers[i],j,FRAG_SIZE,buffer,&filename[0]);
        j += FRAG_SIZE;
    }
    */
    return 0;
}
