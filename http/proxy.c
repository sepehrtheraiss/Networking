#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int fetch_response() {
   int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[4096];
    char *host = "example.com";

    portno = 80;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) perror("ERROR connecting");
    const char * request = "GET / HTTP/1.0\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    n = write(sockfd,request,strlen(request));
    if (n < 0) perror("ERROR writing to socket");
    bzero(buffer,4096);
    n = read(sockfd,buffer,4095);
    if (n < 0) perror("ERROR reading from socket");
    printf("%d\n", (int)strlen(buffer));
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}

#define port 80 // default http port number
int main()
{
	fetch_response();
    return 0;
} 
