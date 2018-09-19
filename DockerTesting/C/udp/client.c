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
    char buffer[2056];
    int sock = socket(AF_INET, SOCK_DGRAM,0);
    perror("socket");
    servaddr.sin_family = AF_INET;
    inet_aton("172.17.0.1",&servaddr.sin_addr);
    //servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(4200);
    char msg[]="msg";
    sendto(sock,msg,sizeof(msg),0,(struct sockaddr*)&servaddr,sizeof(servaddr));
    //connect(sock, (const struct sockaddr *)&servaddr,sizeof(servaddr));
    perror("connect");
//    write(sock,msg,sizeof(msg));
    fprintf(stderr,"waiting\n");
    recvfrom(sock,buffer,2056,0,NULL,NULL);
    puts(buffer);
    int n;
    int total = 0;
    while(total < 85816825){
        n=recvfrom(sock,buffer,2056,0,NULL,NULL);
        total += n;
        printf("total of %i bytes\n",total);
    }
    perror("recvfrom");
    printf("done\n total %i bytes\n",total);
    return 0;
}
