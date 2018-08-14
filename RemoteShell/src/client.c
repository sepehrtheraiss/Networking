#include "../include/ntutils.h"
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
    char* err;
    while(!e)
    {
        fputs("client$ ",stdout);
        if(!fgets(cmd,BUFFSIZE,stdin))
        {
            perror("fgets: ");
        }
        else{
            if((buffer=strchr(cmd,'\n')) != nil )
            {
                *buffer = 0;
            }

            if(strncmp(cmd,"exit",BUFFSIZE) == 0){
                e = 1;
            }
            if((err=sendMSG(&rmtHost,cmd,strnlen(cmd,BUFFSIZE)+1)) != nil){
                fprintf(stderr,"%s",err);
            }
            if(!e){
                if((err=readMSG(&rmtHost,(void**)&buffer)) != nil){
                    fprintf(stderr,"%s",err);
                }
                puts(buffer);
                free(buffer);
                buffer = NULL;
            }
        }
    }
    close(rmtHost.sockfd);
    return 0;
}
