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
    int sock = socket(AF_INET, SOCK_STREAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    bind(sock,(struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(sock,1);

    int cli = accept(sock,(struct sockaddr*)NULL,NULL);
    char msg[1024];
    memset(msg,'A',1024);
    write(cli,msg,sizeof(msg));
    //send(cli,msg,sizeof(msg),0);
    return 0;
}
