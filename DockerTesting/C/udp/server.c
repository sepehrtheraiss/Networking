#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    char buffer[256];
    int sock = socket(AF_INET, SOCK_DGRAM,0);
    servaddr.sin_family = AF_INET;
    //inet_aton("172.17.0.1",&servaddr.sin_addr);//INADDR_ANY;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    bind(sock,(struct sockaddr *) &servaddr, sizeof(servaddr));

    //int cli = accept(sock,(struct sockaddr*)NULL,NULL);
    socklen_t len;
    fprintf(stderr,"waiting\n");
    int n = recvfrom(sock,(char*)buffer,256,0,(struct sockaddr*) &cliaddr,&len);
    fprintf(stderr,"ip: %i\n",cliaddr.sin_addr.s_addr);
    char *ip = inet_ntoa(cliaddr.sin_addr);
    fprintf(stderr,"recvied from [IP:%s\tPort:%i] \n",ip,ntohs(cliaddr.sin_port));
    char msg[]="supergbhirgbhugberuguregergheeeeeeeeeeeyyyyyyyy";
    connect(sock,(struct sockaddr*)&cliaddr,len);
    write(sock,msg,sizeof(msg));
    //sendto(sock,msg,sizeof(msg),MSG_CONFIRM,(struct sockaddr*) &cliaddr,len);
    return 0;
}
