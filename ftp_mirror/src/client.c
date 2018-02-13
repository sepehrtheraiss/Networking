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
#define BUFF_SIZE 128
#define IP_SIZE   16 
int main(int argc, char** argv) {
    /*usage: server-info.txt num-connections filename*/
    if (argc < 4) {
        fprintf(stderr,"usage %s server-info.txt num-connections filename\n", argv[0]);
        exit(0);
    }
    int num_servs; // number of servers in the server-info
    int servs_requested = atoi(argv[2]); // pass in server nums by the user
    char filename[32];
    strcpy(filename,argv[3]);
    FILE* s_file = fopen(argv[1],"r");
    server servers[10];
    num_servs = server_info(s_file,servers);
    fclose(s_file);
    // need to first accept a buffer size
    // to produce the key for ending a connection
    int sockfd;
    struct sockaddr_in serv_addr;
    char buff[BUFF_SIZE];
    int buff_read;

    for(int i=0;i < num_servs && i < servs_requested ;i++)
    {
        /* Create a socket point */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
       
        if (sockfd < 0) {
          perror("ERROR opening socket");
          exit(EXIT_FAILURE);
        }
        //make connection
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = servers[i].IP;
        serv_addr.sin_port = servers[i].port;
       
        /* Now connect to the server */
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(EXIT_FAILURE);
        }
        sprintf(buff,"file_name %s",filename);
        printf("%s\n",buff);
        write(sockfd,buff,50);
       // write(sockfd,"file_name bin/testStack",25);
        buff_read = read(sockfd,buff,BUFF_SIZE);
        write(1,buff,buff_read);
        if(close(sockfd) != 0){
              perror("ERROR on close sockfd");
            exit(EXIT_FAILURE);
        }//end close socket

    }//end for loop

    return 0;
}
