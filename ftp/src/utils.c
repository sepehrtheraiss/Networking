#include "../include/utils.h"
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

int bindIpp(char* ip,int port, struct sockaddr_in* addr)
{
    /* First call to socket() function returns file descriptor */
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   uint32_t IP;
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
   }
   
   /* Initialize socket structure */
   bzero((char *) &addr, sizeof(addr));

   if(inet_pton(AF_INET,ip,&IP)<1){
      perror("ERROR pasring IP address");
      exit(EXIT_FAILURE);
   } 

   /* INADDR_ANY (0.0.0.0) means any address avaiable for binding*/
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = IP; 
   addr.sin_port = htons(port);

   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
   }

    return sockfd;
}
   
void conn(int sockfd,struct sockaddr_in* saddr)
{
   if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      perror("ERROR connecting");
      exit(EXIT_FAILURE);
   }
}
/* void append() this function will be used for huge files such that it calls sendFile * n times because its too big to fit in memory */

/* if size > buff_size => chuncks = buff_size/size*/
off_t sendFile(int sockfd,char* buff,off_t size)
{
    /* take the celing and send n 0's for left overs */
    int chuncks = ceil((double)size / BUFF_SIZE);
    /* if 0 good otherwise positive send n 0's */
    off_t left_over = (chuncks*BUFF_SIZE) - size; 
    printf("read left overs: %i\n"left_over);
    
    off_t n = 0;
    int i=0;
    while(i < chunks)
    {
        n += BUFF_SIZE;
        write(sockfd,buff+(i*BUFF_SIZE),BUFF_SIZE);
        i++;
    }

    if(left_over > 0)
    {
        memset(buff,0,left_over);
        write(sockfd,buff,left_over);
    }

    return BUFF_SIZE+left_over;
}

off_t recvFile(int sockfd,char* buff,int fd)
{

    off_t read = 0;
    int n = 0;
    while( n > 0)
    {
        n = read(sockfd,buff,BUFF_SIZE);
        /* write either to a file or stdout  -1 for none to clear the buffer*/
        if(fd != -1)
            write(fd,buff,n);

        read += n;
    }
        
    return read;
}
