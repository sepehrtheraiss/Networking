#include "../include/header.h"
/*
HTTP/1.0 503 Service Unavailable
Cache-Control: no-cache
Connection: close
Content-Type: text/html

<html><body><h1>503 Service Unavailable</h1>
No server is available to handle this request.
</body></html>


HTTP/1.0 400 Bad request
Cache-Control: no-cache
Connection: close
Content-Type: text/html

<html><body><h1>400 Bad request</h1>
Your browser sent an invalid request.
</body></html>
*/


int main(int argc,char** argv)
{
    if(argc < 3)
    {
        fprintf(stderr, "usage: proxy_server portnumber forbidden_sites\n");
        exit(EXIT_FAILURE);
    }
    FILE* file = fopen(argv[2],"r");
    if(file==NULL)
    {
        fprintf(stderr, "No such file\n");
        exit(EXIT_FAILURE);
    }
    //char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int f_i = 0;
    while ((read = getline(&f_sites[f_i], &len, file)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
       // printf("%s", f_sites[f_i]);
        f_i++;
    }
    f_sites[f_i] = NULL;
    const int myport = atoi(argv[1]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
    }   
    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port = htons(myport);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
    }
    listen(sockfd,LISTEN);
    int stop = 0;
    
    int clisockfd;
    socklen_t clilen;
    while(stop != 1)
    {
        clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (clisockfd < 0) { 
         perror("ERROR on accept");
         exit(EXIT_FAILURE);
        }
        if(fork()==0)
        {
            int s_sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (s_sockfd < 0) perror("ERROR opening socket");
            while(exectute(s_sockfd,clisockfd,cli_addr,serv_addr) > 0); // keep it alive
            if(close(s_sockfd) != 0){
                perror("ERROR close clisockfd");
                exit(EXIT_FAILURE);
            }
            
            if(close(clisockfd) != 0){
                perror("ERROR close clisockfd");
                exit(EXIT_FAILURE);
            }
            exit(1);
        }//end fork

    }// end main while loop
    fclose(file);
    close(sockfd);

    return 0;
} 
