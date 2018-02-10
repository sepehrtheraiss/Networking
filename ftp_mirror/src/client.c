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
#define BUFF_SIZE 512
int main(int argc, char** argv) {
   // need to first accept a buffer size
   // to produce the key for ending a connection
   unsigned int buff_size = BUFF_SIZE;
   int sockfd, portno;
   uint32_t IP;
   struct sockaddr_in serv_addr;
   char buffer[buff_size];
   char serverStackSizeChar [BUFF_SIZE];
   
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
     
   if(close(sockfd) != 0){
      perror("ERROR on close sockfd");
      exit(EXIT_FAILURE);
   }
   return 0;
}
