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

#define buff_size 2048 // 2MB 

int main(int argc, char** argv) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   
   char buffer[buff_size];
   
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

    int flag = 1;
   while(flag != 0){ // while user hasnt typed exit 
	  bzero(buffer,buff_size);
      //printf("Enter command: ");
      char msg[] = "Client$ ";
      n = write(STDOUT_FILENO,msg,sizeof(msg)); 
      //bzero(buffer,buff_size);
    //  fgets(buffer,buff_size,stdin);
      n = read(STDIN_FILENO,buffer,buff_size); // returns counted characters, exclude \0
      //printf("%i %s\n",n,buffer);
      if(n >= buff_size){
         perror("what are you typing bro!");
         exit(EXIT_FAILURE);
      }
      if(strcmp(buffer,"exit\n")==0){ // set flag to zero to exit the loop and end the client
         flag = 0;
      }
      // should be already appenped, but just in case
      buffer[n] = '\0'; // assuming user is not going to type a command longer than 2MB, append \0 after \n
      // Send command to the server
      n = write(sockfd, buffer, n+1);
      // n not -1
      if (n < 0) {
         perror("ERROR writing to socket");
         exit(EXIT_FAILURE);
      }
    // servers response
      bzero(buffer,buff_size);
      char c = 'a';
      int count = 0; // buffer read
      while(c != '\0' && flag != 0){ // read everything, dont need to wait for reading
         // need to check for timeout
         count = read(sockfd,buffer,buff_size); // returns num of chars read up to \n, but \0 is still inserted into the buffer
         if(count < 0){
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
         }
         c = buffer[count]; // to see if \0 has been read
	  }// end while loop
      printf("%s",buffer);
   }//while
   if(close(sockfd) != 0){
      perror("ERROR on close sockfd");
      exit(EXIT_FAILURE);

   }
   return 0;
}
