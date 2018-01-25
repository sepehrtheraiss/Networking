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

#define BUFF_SIZE 512
#define STRING_SIZE 100
int main( int argc, char** argv) {
   int sockfd, clisockfd, portno;
   socklen_t clilen;
   // Setting up rules for the protocol
   char accepted = '0'; // 1 if the client has been accepted as a valid user
   unsigned int buff_size = BUFF_SIZE; // the buffer size can be modified if client agrees to another buffer size; 
   char client_key[STRING_SIZE];// = malloc(sizeof(char)*STRING_SIZE); // clients key to be return once server is done with its operation
   unsigned int key_size =0;
   //unsigned int stack_size = 0; // size will be send to client before servers operation. clients decrementation on stack_size should become zero
   char string[STRING_SIZE]; // for temporaly formated strings to be used for write()
   int flag = 0; // for users exit loop
   char* buffer = malloc(sizeof(char)*BUFF_SIZE);
   stack* stack = stack_init();
   int count = 0; // buffer read
   struct sockaddr_in serv_addr, cli_addr;
   int  n=0; // number of characters read from fread exclude \0, temp number
   FILE* file = NULL; // for popen
   char error[] = "command not found\n";
   int e_size = sizeof(error);
   // protocol tell each other the size of stack + server and client agree on a random number 

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
   
   bzero(buffer,BUFF_SIZE); //init everything to zero

   clilen = sizeof(cli_addr);
   const int x =0; 
   while(x == 0){ // loop forever
      /* Accept actual connection from the client */
      clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      bzero(client_key,STRING_SIZE);

      if (clisockfd < 0) { // client connection
         perror("ERROR on accept");
         exit(EXIT_FAILURE);
      }
      if((n=sprintf(string,"%i",BUFF_SIZE)) == -1){ //formate the message
         perror("sprintf");
         exit(EXIT_FAILURE);
      }
      if(write(clisockfd,string,n) < 0){ // send the size of buffer to client
         perror("write");
         exit(EXIT_FAILURE);
      }     
      // read if client aggress to the buffer
      if((n=read(clisockfd,buffer,BUFF_SIZE)) > 0){
         buffer[n] ='\0';
         if(atoi(buffer) == 1){
            printf("client has accepted BUFF_SIZE of %i\n",BUFF_SIZE); // expecting a accepted : 1
            // continue with second rule, get key
            if((key_size=read(clisockfd,client_key,BUFF_SIZE)) > 0){
               accepted = '1';
               client_key[key_size] = '\0';
               //write(STDOUT_FILENO,client_key,n);
               printf("key:%s\n",client_key);
            }
         }
         else{
            // shrink buff_size, leaving as error handler for now
            perror("atoi failed");
            exit(EXIT_FAILURE);
         }
      }// end buffer accept if

      // remote shell
      flag = 0;
      bzero(buffer,buff_size);
       while(flag != 1){
         // need to check for timeout
         // get command
         // *****reads garbage before even client issues a command ****
         count = read(clisockfd,buffer,buff_size); // returns chars read up until null terminator
         if(count < 1 || count > buff_size){
            perror("ERROR reading command from socket");
            exit(EXIT_FAILURE);
            }
         buffer[count]= '\0';
         write(STDOUT_FILENO,buffer,count); // outputs the received command

         if(strcmp(buffer,"exit\n") == 0){
            flag = 1;
         } // end exit check if
         else{
         n = 1;
         file = popen(buffer,"r"); 
         while((n = fread(buffer,1,buff_size,file)) > 0){
            push_back(stack,buffer,n);
         }
         // printf("%s\n",buffer);
         pclose(file);
         // Write a response to the client, either command not found or doesnt have a response, like exit
         if(stack_size(stack) == 0){
            perror("ERROR on zero buffer size");
            if(write(clisockfd,error,e_size)<1){
               perror("sending error");
               exit(EXIT_FAILURE);
            }
         } 
            // formate stack size
            if((n=sprintf(string,"%i",stack_size(stack))) == -1){ //formate the message
               perror("sprintf");
               exit(EXIT_FAILURE);
            }
            // send stack size
            if(write(clisockfd,string,n)<1){
               perror("sending stack size");
               exit(EXIT_FAILURE);
            }

            while(stack_size(stack) != 0){
               //write(clisockfd,pop_bottom(stack),buff_size);
               if (writeOut(stack,clisockfd) < 1) {
               perror("ERROR writeout to socket");
               exit(EXIT_FAILURE);
               }// end if
               clear(stack); // empties the stack
            }//end while != 0
            write(clisockfd,client_key,key_size); //+1 for \0
         }//else not exit
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
   //free(client_key);
   return EXIT_SUCCESS;
}
