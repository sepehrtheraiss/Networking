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
    FILE* s_file = fopen(argv[1],"r");
    server servers[10];
    server_info(s_file,servers);
    fclose(s_file);
    exit(1);
   // need to first accept a buffer size
   // to produce the key for ending a connection
   int sockfd, portno;
   uint32_t IP;
   struct sockaddr_in serv_addr;
   char buff[BUFF_SIZE];
   int buff_read;
   if (argc < 3) {
      fprintf(stderr,"usage %s IP port\n", argv[0]);
      exit(0);
   }
	
   if(inet_pton(AF_INET,argv[1],&IP)<1){
      perror("ERROR pasring IP address");
      exit(EXIT_FAILURE);
   } 
   portno = atoi(argv[2]);
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
   }
	
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = IP;//htonl(INADDR_LOOPBACK);
   serv_addr.sin_port = htons(portno);
   
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(EXIT_FAILURE);
   }
   write(sockfd,"file_name bin/testStack",25);
   buff_read = read(sockfd,buff,BUFF_SIZE);
   write(1,buff,buff_read);
   if(close(sockfd) != 0){
      perror("ERROR on close sockfd");
      exit(EXIT_FAILURE);
   }
   return 0;
}
