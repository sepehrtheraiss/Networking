#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "../include/header.h"
// takes in a file descriptor
// converts string to unit32_t and int 
// writes to IP[] and port[]
// returns number of server read from the file ( based on \n )
int server_info(FILE *file,struct server* s)
{
    int n = 0; // num serves
    char *line = NULL;
    char IP[IP_SIZE+1];
    char port[PORT_SIZE+1];
    size_t linecap = 0;
    ssize_t linelen;
    int i;
    int j;
    while ((linelen = getline(&line, &linecap, file)) > 0)
    {
        i = 0;
        // IP
        while((int)line[i]!= 32){++i;}
        memcpy(&IP[0],&line[0],i);
        IP[i] = 0;
        if(inet_pton(AF_INET,&IP[0],&(s[n].IP))<1){
             perror("ERROR pasring IP address");
            exit(EXIT_FAILURE);
        } 
        j=i;
        while(line[i]!='\n'){++i;}
        memcpy(&port[0],&line[j+1],i-j-1);
        port[i-j] = 0;
        s[n].port = htons(atoi(&port[0]));
        n++; // num servers
    }
    return n;
}


int getFileSize(server* servers,char* filename,int num_servs,int servs_req)
{
    // to produce the key for ending a connection
    int sockfd;
    struct sockaddr_in serv_addr;
    char buff[BUFF_SIZE];
    int buff_read;
    int sup =0 ;//number of up servers
    
    for(int i=0;i < num_servs && sup < servs_req ;i++)
    {
           /* Create a socket point */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
       
        if (sockfd < 0) {
          perror("ERROR opening socket");
          exit(EXIT_FAILURE);
        }
        //make connection
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = servers[i].IP;
        serv_addr.sin_port = servers[i].port;
       
        /* Now connect to the server */
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
        }
        else
        {
            sup++;
         sprintf(buff,"file_name %s",filename);
         printf("%s\n",buff);
         write(sockfd,buff,50);
        // write(sockfd,"file_name bin/testStack",25);
            buff_read = read(sockfd,buff,BUFF_SIZE);
            //write(1,buff,buff_read);
         if(close(sockfd) != 0){
                perror("ERROR on close sockfd");
                exit(EXIT_FAILURE);
            }//end close socket
            buff[buff_read] = 0;
            return atoi(buff);
        }//end else made a connections
    }//end for loop
    return 0;
}
void read_offset(FILE *file,int off, int bytes,char *buffer)
{
   fseek(file,off,SEEK_SET);
   fread(buffer,1,bytes,file);
}
// attemps to write a message and read the response if message recieved was correct
// returns 1 on true the users message has ok else 0 and on waittime exeteeded
int writeit(int fd,char* buffer,int size)
{
    if(write(fd,buffer,size) < 0)
    {
        perror("error on write");
        exit(1);
    }
    if(read(fd,buffer,size) > 0)
    {
        return 1;
    }
    else
    {
        perror("error on read");
        exit(1);
    }
    return 0;
}

// reads then sends ok
// returns reads read
int readit(int fd,char* buffer,int size)
{
    int n=0;
    if((n=read(fd,buffer,size)) > 0)
    {
 
        if(write(fd,buffer,size) < 0)
        {
            perror("error on write");
            return 0;
        }
        
    }
    else
    {
        perror("error on read");
        exit(1);
    }
    return n;
}
