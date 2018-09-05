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
                    char* err;
                    if((err=readMSG(hosts[cli%MAXCLI].rmtHost,(void**)&buffer)) == nil)
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
                                if((err=sendMSG(hosts[cli%MAXCLI].rmtHost,buffer,st.st_size+1)) != nil){
                                    fprintf(stderr,"%s",err);
                                }
                                close(fd);
                            }
                            else{
                                perror("system failed");
                            }
                        }
                        else{
                            e = 1;
                            if(remove(pwd) < 0)
                            {
                                perror("remove: ");
                            }
                        }
                        free(buffer);
                        buffer = NULL;
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
