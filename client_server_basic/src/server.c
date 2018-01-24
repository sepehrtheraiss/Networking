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
#include "../header/stack.h"

#define buff_size 512//2048 // 2MB 

int main( int argc, char** argv) {
   int sockfd, clisockfd, portno;
   socklen_t clilen;
   char* buffer = malloc(sizeof(char)*buff_size);
   stack* stack = stack_init();
   struct sockaddr_in serv_addr, cli_addr;
   int  n=0; // number of characters read from fread exclude \0
   //n=0; // just to shut up the complaning
   int buff_read=0;//size of buffer read, sum of all n's, if command return or was an error
   FILE* file = NULL; // for popen
   char error[] = "command not found\n";
   int e_size = sizeof(error);
   

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
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
const int x =0;
while(x == 0){
   /* Accept actual connection from the client */
   clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  // printf("my newsocket file descriptor %i and orign socket is:%i\n",newsockfd,sockfd);
	
   if (clisockfd < 0) {
      perror("ERROR on accept");
      exit(EXIT_FAILURE);
   }
   while(strcmp(buffer,"exit\n") != 0){
      bzero(buffer,buff_size);
      char c = 'a';
      int count = 0; // buffer read
      while(c != '\0'){ // read everything
         // need to check for timeout
         count = read(clisockfd,buffer,buff_size); // returns chars read up until null terminator
         if(count < 0){
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
         }
         c = buffer[count]; // to see if \0 has been read
      }//end
      write(STDOUT_FILENO,buffer,count); // outputs the received command

      if(strcmp(buffer,"exit\n") != 0){
        buff_read = 0;
         n = 1;
         file = popen(buffer,"r"); 
         while((n = fread(buffer,1,buff_size,file)) > 0){
            buff_read += n;
            if(n > 0){// probably dont need it
               push_back(stack,buffer);
            }
         }
         // printf("%s\n",buffer);
         pclose(file);
         /* Write a response to the client, need to check for oversized file*/
         if(buff_read < 1){
            perror("ERROR on zero buffer size");
            n = write(clisockfd,error,e_size);
         }
         else{
            //printStack(stdout,stack);
            while(stack_size(stack) != 0){
               write(STDOUT_FILENO,bottom(stack),buff_size);
               n = write(clisockfd,pop_bottom(stack),buff_size);
               if (n < 0) {
               perror("ERROR writing to socket");
               exit(EXIT_FAILURE);
               }// end if
            }//end while
         }//end else 
      } // end exit check if
  

 }// end inner server while loop, waiting for exit command
   if(close(clisockfd) != 0){
      perror("ERROR close clisockfd");
      exit(EXIT_FAILURE);
   }
}//end outter while main server loop to accept
   if(close(sockfd) != 0){
      perror("ERROR close sockfd");
      exit(EXIT_FAILURE);  
   }
   stack_deinit(stack);
   free(buffer);
   return EXIT_SUCCESS;
}
