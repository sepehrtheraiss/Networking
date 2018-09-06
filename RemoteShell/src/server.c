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

    struct host* localhost = initHost(argv[1],argv[2],TCP); 
    if(localhost == nil )
    {
        fprintf(stderr,"initHost failed\n");
        return -1;
    }

    struct hosts{
        /* Each pid is assosiated with a remote host, done on accept */
        pid_t pid;
        struct host* rmtHost;
    }hosts[MAXCLI];

    char pwd[BUFFSIZE];
    int cli = 0;
    pid_t pid = -1;
    if(!getcwd(pwd,BUFFSIZE))
    {
        perror("getpwd: ");
    }

    fprintf(stderr,"[pwd: %s]\n",pwd);
    fprintf(stderr,"[server pid: %i]\n",getpid());
    while(true){
        if(cli <= MAXCLI)
        {
            if(pid != 0){
                pid = acceptSession(localhost,&hosts[cli%MAXCLI].rmtHost);
                hosts[cli%MAXCLI].pid = pid;
                if(pid != 0) cli++;
                fprintf(stderr,"[pid: %i]\n",pid);
            }

            if(pid == 0){
                bool e = 0;
                while(!e){
                    FILE* f = NULL;
                    char buffer[BUFFSIZE];
                    char* err = nil;
                    size_t n;
                    uint32_t id = 0;
                    uint8_t state;

                    if((n=readMSG(hosts[cli%MAXCLI].rmtHost, &id, &state, buffer)) != -1)
                    {
                        fprintf(stderr,"[cmd: %s]\n",buffer);
                        if(strcmp(buffer,"exit") != 0)
                        {
                            if((f = popen(buffer,"r")) != nil)
                            {
                                if((err=sendMSG(hosts[cli%MAXCLI].rmtHost, id, START, n, buffer)) != nil){
                                    fprintf(stderr,"%s",err);
                                }
                                else{
                                    while(!feof(f)){
                                        n = fread(buffer, BUFFSIZE, 1, f);
                                      //  sendMSG(buffer,n);
                                    }
                                    pclose(f);
                                }
                            }
                            else{
                                fprintf(stderr,"popen failed\n");
                            }
                        }
                        else{
                            e = 1;
                        }
                    }
                    else{
                        fprintf(stderr,"%s",err);
                    }
                }
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            fprintf(stderr,"Reached max client acceptance of %i\n",MAXCLI);
            // Block, wait until a child process has been terminated
            pid_t rt_pid;
            int stat;
            rt_pid = wait(&stat); 
            fprintf(stderr,"rt_pid: %i WIFEXITED: %i\n",rt_pid,WIFEXITED(stat));
            cli--;
        }
    }
    return 0;
}
