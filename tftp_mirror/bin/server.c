#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#define BUFF_LEN 512
#define PORT 69
int main()
{
    struct sockaddr_in si_me, si_other;
    int s,i,slen=sizeof(si_other);
    char buff[BUFF_LEN];

    if((s=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)<0))
    {
        perror("socket");
        exit(1);
    }
    memset((char*)&si_me,0,sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(s,(struct sockaddr *)&si_me,sizeof(si_me)) < 0)
    {
        perror("bind");
        exit(1);
    }
    recvfrom(s, buff, BUFF_LEN, 0, (struct sockaddr *)&si_other,(unsigned int*)&slen);
    //read(s,buff,BUFF_LEN);
    printf("msg: %s\n",buff);
    close(s);
    return 0;
}
