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
#include "../include/header.h"
#define R_READ    128
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
   
    char buff[BUFF_SIZE]; // will be used for the original copy of the request and for writing 
    int buff_read;
    char r_buff[R_READ]; // read_buffer for requests, temporaly copies string from buff
    unsigned int off_set; // file off set 
    unsigned int bytes; // bytes to read
    int fd;
    struct stat f_stat;
    off_t file_size;
    char PATH[BUFF_SIZE];
    FILE* pwd = popen("pwd","r");
    buff_read = fread(PATH,1,BUFF_SIZE,pwd);
    pclose(pwd);
    if(buff_read+1 > BUFF_SIZE){ // +1 for null
        perror("buff read > buff size");
        exit(1);
    }
    // appending / to the path
    if((int)PATH[buff_read-1] == 10) // new line feed
    {
        PATH[buff_read-1] = '/';
        PATH[buff_read] = 0;
    }
    else
    {
        PATH[buff_read] = '/';
        PATH[buff_read+1] = 0;
    }
   clilen = sizeof(cli_addr);
   while(1){ // loop forever
      /* Accept actual connection from the client */
      clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

      if (clisockfd < 0) { // client connection failed
         perror("ERROR on accept");
         //exit(EXIT_FAILURE);
      }
      // connection established
      else
      {
        if((buff_read = read(clisockfd,buff,R_READ)) < 0){
            perror("ERROR on client read");
        }
        else
        {
            buff[buff_read] = 0;
            // making sure that the strcmp only compares the wanted characters
            memcpy(r_buff,&buff[0],9);
            r_buff[9] = 0; //null terminator
            if(strcmp("file_name",r_buff) == 0){
                r_buff[0] = 0;
                // usage: file_name filename. opens the file then returns the file size
                // get last index of filename
                for(int i =10;buff[i] != 0;i++)
                {
                    if(buff[i+1] == 0)
                    {
                        memcpy(r_buff,&buff[10],i);
                        r_buff[i+1] = 0;
                    
                    }
                }

               // memcpy(r_buff,&buff[10],54);
                //r_buff[54] = 0;
                strcpy(buff,PATH);// add path
                strcat(buff,r_buff);// add filename
                fd = open(buff,O_RDONLY);
                if(fstat(fd,&f_stat) == -1){
                    perror("ERROR on fstat");
                    write(sockfd,"0",2);
                }
                else 
                {
                    bzero(buff,32);
                    sprintf(buff,"%lli",f_stat.st_size); 
                    write(clisockfd,buff,32);
                    file_size = f_stat.st_size; 
                }
                close(fd);
            }
            // 
            memcpy(r_buff,&buff,6);
            r_buff[6] = 0; //null terminator
            if(strcmp("offset",r_buff) == 0)
            {
                // usage: offset filename(x,y)
                // find (x,y) 
                // filename
                int i = 7;
                while((int)buff[i] != (int)'(')
                {
                    r_buff[i-7] = buff[i];
                    ++i;
                }
                r_buff[i] = 0;
                FILE* file = fopen(r_buff,"r"); // would it work since r_buff will change
                // off set
                i++;// cursor after (
                int j = 0;
                while((int)buff[i] != (int)',')
                {
                    r_buff[j++] = buff[i++];
                }
                r_buff[i] = 0;
                off_set = atoi(r_buff);
                i++;// after ','
                j=0;
                while((int)buff[i] != (int)')')
                {
                    r_buff[j++]= buff[i++];
                }
                r_buff[i] = 0;
                bytes = atoi(r_buff);
                if(bytes > BUFF_SIZE)
                {
                    char error[] = "fragment size should be < 512\n";
                    write(clisockfd,error,sizeof(error));
                }
                else
                {
                    read_offset(file,off_set,bytes,buff);
                    write(clisockfd,buff,bytes);
                }
                printf("offset (%i,%i)\n",off_set,bytes);

            }
       }// end else success reading from client
      }//end else connection established with the client
      
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
