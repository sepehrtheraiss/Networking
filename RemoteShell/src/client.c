#include "../include/ntutils.h"
#include <string.h>
int main(int argc,char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr,"[Usage: IP port]\n");
        return -1;
    }

    struct host* rmtHost = initHost(argv[1], argv[2], CLITCP);
    if (rmtHost == nil)
    {
        fprintf(stderr,"initHost failed\n");
        return -1;
    }

    char cmd[BUFFSIZE];
    char* s;
    char buffer[BUFFSIZE+1];
    bool e = false;
    uint16_t id = 0;
    int state; 
    int n;
    while(!e)
    {
        fputs("client$ ",stdout);
        memset(buffer,0,BUFFSIZE);
        if(!fgets(cmd,BUFFSIZE,stdin))
        {
            perror("fgets: ");
        }
        else{
            if((s=strchr(cmd,'\n')) != nil ){
                *s = 0;
            }

            if(strncmp(cmd,"exit",BUFFSIZE) == 0){
                e = true;
            }
            if(sendMSG(rmtHost, id, START, strnlen(cmd,BUFFSIZE), cmd)){
                state = START;
                if(!e){
                    while(state != END){
                        printf("read\n");
                        if((n=readMSG(rmtHost, &id, &state, &buffer)) > 0){
                            write(1, buffer, n);
                        }
                    }
                }
            }
        }
    }
    closeHost(rmtHost);
    return 0;
}
