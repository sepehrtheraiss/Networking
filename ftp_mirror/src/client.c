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
#include "../include/header.h"
int main(int argc, char** argv) {
    /*usage: server-info.txt num-connections filename*/
    if (argc < 4) {
        fprintf(stderr,"usage %s server-info.txt num-connections filename\n", argv[0]);
        exit(0);
    }
    server servers[10];
    char buffer[BUFF_SIZE];
    unsigned int fileSize;
    int servs_req = atoi(argv[2]); // pass in server nums by the user
    char filename[32];
    strcpy(filename,argv[3]);
    FILE* s_file = fopen(argv[1],"r");
    int num_servs = server_info(s_file,servers); // number of servers in the server-info
    fclose(s_file);
    // gets the file size from the first responsive server
    if((fileSize = getFileSize(servers,filename,num_servs,servs_req)) < 1)
    {
        perror("file either does not exist or all the servers are down");
        exit(1);
    }
    // fragment size
    const unsigned int FRAG_SIZE = fileSize / servs_req; // ?? at compile time
    printf("frag: %i \n",FRAG_SIZE);
    
    // seek and read
    sendOffsetRead(&servers[0],0,FRAG_SIZE,buffer,&filename[0]);
    return 0;
}
