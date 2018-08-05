#include "ntutils.h"
int main(int argc,char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr,"[Usage: IP port]\n");
        return -1;
    }

    struct host rmtHost = {
        .IPv4Str = argv[1],
        .portStr = argv[2],
        .proto = CLITCP
    };
    if (!initSock(&rmtHost))
    {
        fprintf(stderr,"error: initSock()\n");
        return -1;
    }
    char msg[]="hello from the other side";
    sendMSG(&rmtHost,msg,sizeof(msg));
    close(rmtHost.sockfd);
    return 0;
}
