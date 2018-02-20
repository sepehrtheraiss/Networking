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
    struct sockaddr_in si_other;
    int s,i,slen=sizeof(si_other);
    char buff[] = "something bs";
    if((s=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<1)
    {
        exit(1);
    }
    memset((char*)&si_other,0,sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) {
          fprintf(stderr, "inet_aton() failed\n");
          exit(1);
        }
       sendto(s, buff, BUFF_LEN, 0,(struct sockaddr *) &si_other, slen);
    //write(s,buff,BUFF_LEN);
    close(s);
return 0;
}
