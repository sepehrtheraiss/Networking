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

#define buff_size 2048 // 2MB 

int main( int argc, char** argv) {
   int sockfd, clisockfd, portno;
   socklen_t clilen;
   char buffer[buff_size];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   

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
   printf("my loopback address is: %u\n",INADDR_LOOPBACK );
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
   
while(1){
   /* Accept actual connection from the client */
   clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  // printf("my newsocket file descriptor %i and orign socket is:%i\n",newsockfd,sockfd);
	
   if (clisockfd < 0) {
      perror("ERROR on accept");
      exit(EXIT_FAILURE);
   }

   while(strcmp(buffer,"exit") != 0){
      // init buffer to $'s
      for(int i =0;i<buff_size;i++){
         buffer[i] = '$';
      }
      char c = 'a';
      int count = 0; // buffer read
      while(c != '$'){ // read everything
         // need to check for timeout
         count = read(clisockfd,buffer,buff_size); // returns chars read up until null terminator
         if(count < 0){
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
         }
         c = buffer[count]; // to see if \0 has been read
      }
      write(STDOUT_FILENO,buffer,count);
   //printf("Here is the command: %s\n",buffer);
   
   // remove \n from buffer
   int i=0;
   while(buffer[i] != '\n' && i < buff_size){ // just in case no infinite loop
      i++;
   }   
   buffer[i] = '\0';
   char redirect[] = " > temp_file";
   char superBuff[buff_size];
   strcpy(superBuff,buffer);
   strcat(superBuff,redirect);
   system(superBuff);// execute command then redirect it to a file

   int fd = open("temp_file",O_RDONLY);
   if(fd == -1){
         perror("open\n");
         exit(EXIT_FAILURE);
      }

   char * str = NULL;
   struct stat s;
   off_t buff_size_byte; // file size in bytes for the file descriptor

   if(fstat(fd,&s) != -1){ // get files status
      buff_size_byte = s.st_size; // size of file
      str = malloc(sizeof(char)*buff_size_byte);  // allocate string of size file
   }
   if(read(fd,str,s.st_size) < 0){ // 0 means the there is no more to read, -1 means an error has occured
      perror("read\n");
      exit(EXIT_FAILURE);
   }   
   /* Write a response to the client, need to check for oversized file*/
   n = write(clisockfd,str,buff_size_byte);
   free(str);
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(EXIT_FAILURE);
   }
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

   return EXIT_SUCCESS;
}
