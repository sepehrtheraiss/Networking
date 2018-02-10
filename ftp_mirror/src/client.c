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
#include "../header/stack.h"
#include "../header/header.h"
#define BUFF_SIZE 512
#define STRING_SIZE 100
int main(int argc, char** argv) {
   // need to first accept a buffer size
   // to produce the key for ending a connection
   unsigned int buff_size = BUFF_SIZE;
   char msg[] = "Client$ ";

   char accepted = '1';
   char key[] = "f2567239h6015"; 
   const int key_size = sizeof(key);
   int sockfd, portno, n;
   uint32_t IP;
   struct sockaddr_in serv_addr;
   char buffer[buff_size];
   char serverStackSizeChar [BUFF_SIZE];
   int serverStackSize = 0;
   stack* stack = stack_init();
   
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
   
   /* Now ask for a message from the user, this message
      * will be read by server
   */
   // get buffer size
   printf("reading buffer: %i\n",readit(sockfd,buffer,buff_size));
   /*
   if(read(sockfd,buffer,buff_size)<1){
      perror("read buffer");
      exit(EXIT_FAILURE);
   }
   */
   // just accept for now
   printf("writeting accept: %i\n",writeit(sockfd,&accepted,2));
   /*
   if(write(sockfd,&accepted,2)<1){
      perror("write buffer");
      exit(EXIT_FAILURE);  
   }
   */
   // generate key, generate a manual custome key
   // send key
   printf("writing key: %i\n",writeit(sockfd,&(key[0]),key_size));
   /*
   if(write(sockfd,key,key_size)<1){
      perror("write key");
      exit(EXIT_FAILURE);  
   }
*/
   int flag = 1;
   bzero(buffer,buff_size);
   //n=0;
   while(flag != 0){ // while user hasnt typed exit 
      write(STDOUT_FILENO,msg,8); //printf("Client$: ");
      // ***get users command input****
      n = read(STDIN_FILENO,buffer,buff_size); // returns counted characters, exclude \0
    printf("your shit: %s\n",buffer);
      if(n >= buff_size){
         perror("what are you typing bro!");
         exit(EXIT_FAILURE);
      }
      buffer[n] ='\0';

      if(strcmp(buffer,"exit\n")==0 || strcmp(buffer,"exit")==0){ // set flag to zero to exit the loop and end the client
         flag = 0;
         if(write(sockfd, buffer, n) < 1) {
            perror("ERROR writing exit to socket");
            exit(EXIT_FAILURE);
         }
         printf("exiting remote shell\n");
      }
      else{
         // **Send command to the server**
         n = write(sockfd, buffer, n);// n = num characters read from user
         // n not -1
         if (n < 1) {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
         }
         // try read stack size
         if((n=read(sockfd,serverStackSizeChar,buff_size)) < 1){
            perror("ERROR reading from server stack size");
            exit(EXIT_FAILURE);  
         }
         serverStackSizeChar[n]='\0';
         //if valid command
         //get the stack size
         //read until stack sizes matches server stack size
         if(strcmp(serverStackSizeChar,"command not found\n") == 0 ){
            write(STDOUT_FILENO,serverStackSizeChar,n);
          }// end servers response if not valid
          else{//if stack size valid
               // get stack size
              if((serverStackSize = atoi(serverStackSizeChar)) >= buff_size){
                  perror("ERROR stack size too big");
                  exit(EXIT_FAILURE);
               }
               // get servers respond
                int count = 0; // buffer read
                while(stack_size(stack) != serverStackSize){
                   // returns num of chars read up to \n, but \0 is still inserted into the buffer
                  if((count = read(sockfd,buffer,buff_size)) < 1){
                      perror("ERROR reading messages from socket");
                      exit(EXIT_FAILURE);
                   }
                   if(stack_size(stack) == serverStackSize-1){ // last string needs to be added a \0
                        buffer[count]=0;
                   }
                    push_back(stack,buffer,count);
                }//end while stack size check

            // get key
            printf("reading key: %i\n",readit(sockfd,buffer,key_size));
            /*
            if((n=read(sockfd,buffer,key_size)) < 1){
                  perror("ERROR reading key from socket");
                  exit(EXIT_FAILURE);
            }
            */
            buffer[n]='\0';
            // if mathced
            if(strcmp(buffer,key) != 0){
               printf("keys dont match\n");
               flag = 0;
            }
            else{
               printStack(stdout,stack);
               clear(stack);
            }
         }//end else if stack size valid

         }//end else not exit
   }//end flag loop
   
   if(close(sockfd) != 0){
      perror("ERROR on close sockfd");
      exit(EXIT_FAILURE);
   }
   stack_deinit(stack);
   return 0;
}
