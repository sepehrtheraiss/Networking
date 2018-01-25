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
#include "../header/stack.h"
#define BUFF_SIZE 512
int main(int argc, char** argv) {
   // need to first accept a buffer size
   // to produce the key for ending a connection
   unsigned int buff_size = BUFF_SIZE;
   char accepted = '1';
   const char* key = "f2567239h6015\0"; 
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   char buffer[buff_size];
   char serverStackSizeChar [100];
   unsigned int serverStackSize = 0;
   stack* stack = stack_init();
   
   if (argc < 2) {
      fprintf(stderr,"usage %s port\n", argv[0]);
      exit(0);
   }
	
   portno = atoi(argv[1]);
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
   }
	
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   serv_addr.sin_port = htons(portno);
   
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(EXIT_FAILURE);
   }
   
   /* Now ask for a message from the user, this message
      * will be read by server
   */
   // get buffer size
   if(read(sockfd,buffer,buff_size)<1){
      perror("read buffer");
      exit(EXIT_FAILURE);
   }
   // just accept for now
   if(write(sockfd,&accepted,2)<1){
      perror("write buffer");
      exit(EXIT_FAILURE);  
   }
   // generate key, generate a manual custome key
   if(write(sockfd,key,buff_size)<1){
      perror("write key");
      exit(EXIT_FAILURE);  
   }

   int flag = 1;
   while(flag != 0){ // while user hasnt typed exit 
	  bzero(buffer,buff_size);
      //printf("Enter command: ");
      char msg[] = "Client$ ";
      n = write(STDOUT_FILENO,msg,sizeof(msg)); 
      // get users input
      n = read(STDIN_FILENO,buffer,buff_size); // returns counted characters, exclude \0
      //printf("%i %s\n",n,buffer);
      if(n >= buff_size){
         perror("what are you typing bro!");
         exit(EXIT_FAILURE);
      }
      if(strcmp(buffer,"exit\n")==0){ // set flag to zero to exit the loop and end the client
         flag = 0;
         printf("exiting remote shell\n");
      }
      else{
         //buffer[n] = '\0';
         // **Send command to the server**
         n = write(sockfd, buffer, n+1); // +1 for \0
         // n not -1
         if (n < 1) {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
         }
         // try read stack size
         if(read(sockfd,serverStackSizeChar,100) < 1){
            perror("ERROR reading from server stack size");
            exit(EXIT_FAILURE);  
         }
         if(strcmp(serverStackSizeChar,"command not found\n") !=0 ){
            serverStackSize = atoi(serverStackSizeChar);
            int count = 0; // buffer read
            while(stack_size(stack) != serverStackSize){
               // returns num of chars read up to \n, but \0 is still inserted into the buffer
               if((count = read(sockfd,buffer,buff_size)) < 1){
                  perror("ERROR reading messages from socket");
                  exit(EXIT_FAILURE);
               } 
               push_back(stack,buffer,count);
            }

            if(read(sockfd,buffer,100) < 1){
                  perror("ERROR reading key from socket");
                  exit(EXIT_FAILURE);
            }
            if(strcmp(buffer,key) != 0){
               flag = 0;
            }
            printStack(stdout,stack);
         }//end if command not found
      }// end if user typed exit
   }// while user hasnt typed exit 
   
   if(close(sockfd) != 0){
      perror("ERROR on close sockfd");
      exit(EXIT_FAILURE);
   }
   stack_deinit(stack);
   return 0;
}
