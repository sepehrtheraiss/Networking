#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    char buffer[256];
//    int sock = socket(AF_INET, SOCK_STREAM,0);
    int sock = socket(AF_INET, SOCK_DGRAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    bind(sock,(struct sockaddr *) &servaddr, sizeof(servaddr));
    //listen(sock,1);

    //int cli = accept(sock,(struct sockaddr*)NULL,NULL);
    int len;
    int n = recvfrom(sock,(char*)buffer,256,MSG_WAITALL,(struct sockaddr*) &cliaddr,&len);
    char msg[]="supergbhirgbhugberuguregergheeeeeeeeeeeyyyyyyyy";
    //write(cli,msg,sizeof(msg));
    sendto(sock,msg,sizeof(msg),MSG_CONFIRM,(struct sockaddr*) &cliaddr,len);
    return 0;
}
