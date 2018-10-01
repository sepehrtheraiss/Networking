#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
int main()
{
    pid_t pid = fork();
    if(pid != 0){
        char buffer[256];
        struct sockaddr_in servaddr, cliaddr;
        int sock = socket(AF_INET, SOCK_DGRAM,0);
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr =  htonl(INADDR_LOOPBACK);;
        servaddr.sin_port = htons(8080);

        bind(sock,(struct sockaddr *) &servaddr, sizeof(servaddr));
        fprintf(stderr,"server\n");
        socklen_t len;
        int n = recvfrom(sock,(char*)buffer,256,0,(struct sockaddr*)&cliaddr,&len);
        buffer[n]=0;
        fprintf(stderr,"server recv: %s cli len: %i\n",buffer,len);
        char msg[]="hello from server ddweiudhiuewdheiuwdheiuwdheuwidheiuwfhewiufgewuifgweiufgweuifgewiufgewuifgweuifgweuifgwuifgwuifgweiufgweiufgweiufgewiiuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu";
        sendto(sock,(const char*)msg,sizeof(msg),0,(const struct sockaddr*) &cliaddr,len);
        printf("here\n");
    }
    else if(pid == 0){
        int n;
        char sendline[]="some message";
        char recvline[256+1];
        struct sockaddr_in servaddr, cliaddr;
        int sock = socket(AF_INET, SOCK_DGRAM,0);
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr =  htonl(INADDR_LOOPBACK);
        servaddr.sin_port = htons(8080);

        fprintf(stderr,"client\n");
        connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr));
        write(sock,sendline,sizeof(sendline));
        if((n = read(sock,recvline,32)) < 0)
        {
            perror("read fucked");
        }
        recvline[n] = 0;
        fprintf(stderr,"read %i bytes\n",n);
        puts(recvline);
    }
    return 0;
}
