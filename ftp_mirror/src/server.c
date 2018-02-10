#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFF_SIZE 512
int main( int argc, char** argv) {
   int sockfd, clisockfd, portno;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

   if (argc < 2) {
      fprintf(stderr,"usage %s port\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   
   /* First call to socket() function returns file descriptor */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]);
   
   /*INADDR_LOOPBACK (127.0.0.1) always refers to the local host via the loopback device*/
   /* INADDR_ANY (0.0.0.0) means any address for binding*/
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);//.s_addr = INADDR_ANY; 
   serv_addr.sin_port = htons(portno);

   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
   }
   listen(sockfd,5);

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   

   clilen = sizeof(cli_addr);
   while(1){ // loop forever
      /* Accept actual connection from the client */
      clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

      if (clisockfd < 0) { // client connection
         perror("ERROR on accept");
         exit(EXIT_FAILURE);
      }
      if(close(clisockfd) != 0){
         perror("ERROR close clisockfd");
         exit(EXIT_FAILURE);
      }
   }//end outter while main server loop to accept

   if(close(sockfd) != 0){
      perror("ERROR close sockfd");
      exit(EXIT_FAILURE);  
   }
   return EXIT_SUCCESS;
}
