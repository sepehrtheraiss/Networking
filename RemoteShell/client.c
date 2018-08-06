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
    char cmd[BUFFSIZE];
    char* buffer;
    bool e = 0;
    while(!e)
    {
        scanf("%s",cmd);
        if(strncmp(cmd,"exit",BUFFSIZE) == 0){
            e = 1;
        }
        sendMSG(&rmtHost,cmd,strnlen(cmd,BUFFSIZE)+1);
        if(!e){
            readMSG(&rmtHost,(void**)&buffer);
            puts(buffer);
            free(buffer);
            buffer = NULL;
        }

    }
    close(rmtHost.sockfd);
    return 0;
}
