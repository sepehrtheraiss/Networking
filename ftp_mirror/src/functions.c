#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

// attemps to write a message and read the response if message recieved was correct
// returns 1 on true the users message has ok else 0 and on waittime exeteeded
int writeit(int fd,char* buffer,int size)
{
    if(write(fd,buffer,size) < 0)
    {
        perror("error on write");
        exit(1);
    }
    if(read(fd,buffer,size) > 0)
    {
        return 1;
    }
    else
    {
        perror("error on read");
        exit(1);
    }
    return 0;
}

// reads then sends ok
// returns reads read
int readit(int fd,char* buffer,int size)
{
    int n=0;
    if((n=read(fd,buffer,size)) > 0)
    {
 
        if(write(fd,buffer,size) < 0)
        {
            perror("error on write");
            return 0;
        }
        
    }
    else
    {
        perror("error on read");
        exit(1);
    }
    return n;
}
