#include "../include/ntutils.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int main(int argc,char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr,"Usage: IP port\n");
        return -1;
    }

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
    char pwd[BUFFSIZE];
    if(!getcwd(pwd,BUFFSIZE))
    {
        perror("getpwd: ");
    }
    fprintf(stderr,"[pwd: %s]\n",pwd);
    printf("[server pid: %i]\n",getpid());
    while(true){
        pid_t pid = acceptSession(&localhost,rmtHost);
        fprintf(stderr,"[pid: %i]\n",pid);

        if(pid == 0){
            snprintf(pwd,BUFFSIZE,"%s/tmp/tmp.%i",pwd,getpid());
            int fd = open(pwd, O_CREAT | O_TRUNC |  O_WRONLY, 0000666);
            if(fd < 0)
            {
                perror("creat tmp: ");
                exit(1);
            }
            close(fd);
            bool e = 0;
            while(!e){
                char* buffer = NULL;
                char redirect[BUFFSIZE];
                if(readMSG(rmtHost,(void**)&buffer) == nil)
                {
                    fprintf(stderr,"[cmd: %s]\n",buffer);
                    if(strcmp(buffer,"exit") != 0)
                    {
                        snprintf(redirect,BUFFSIZE,"%s &> %s",buffer,pwd);
                        if(system(redirect) > -1){
                            fd = open(pwd,O_RDONLY);
                            struct stat st;
                            if(fstat(fd,&st)<0)
                            {
                                perror("fstat: ");
                            }
                            buffer = realloc(buffer,st.st_size);
                            read(fd,buffer,st.st_size);
                            buffer[st.st_size] = 0;
                            sendMSG(rmtHost,buffer,st.st_size+1);
                            close(fd);
                        }
                        else{
                            perror("system failed");
                        }
                    }
                    else{
                        e = 1;
                    }
                    free(buffer);
                    buffer = NULL;
                }
            }
            exit(EXIT_SUCCESS);
        }
    }
    closeRmtHost(rmtHost);
    close(localhost.sockfd);
    return 0;
}
