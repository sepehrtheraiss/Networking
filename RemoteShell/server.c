#include "ntutils.h"
int main(int argc,char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr,"Usage: IP port\n");
        return -1;
    }

    printf("[server pid: %i]\n",getpid());

    struct host localhost = {
        .IPv4Str = argv[1],
        .portStr = argv[2],
        .proto = TCP
    };
    if (!initSock(&localhost))
    {
        fprintf(stderr,"error: initSock()\n");
        return -1;
    }
    listen(localhost.sockfd,BACKLOG);
    struct host rmtHost[BACKLOG];
    while(true){
        pid_t pid = acceptSession(&localhost,rmtHost);
        printf("[pid: %i]\n",pid);
        if(pid == 0){
            char buffer[BUFFSIZE];
            readMSG(rmtHost,buffer);
            printf("%s\n",buffer);
            exit(EXIT_SUCCESS);
        }
        /*
        else{
            wait(nil); 
        }
        */
    }
    closeRmtHost(rmtHost);
    close(localhost.sockfd);
    return 0;
}
