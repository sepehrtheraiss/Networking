//-------------------------------------------------
// Sepehr Raissian
// Sraissia@ucsc.edu
// CMPE156 
// header.h
//-------------------------------------------------

#ifndef _HEADER_H_INCLUDE_
#define _HEADER_H_INCLUDE_
#include <stdlib.h>
#define IP_SIZE 16
#define PORT_SIZE 4
typedef struct server{
    char IP[IP_SIZE+1];
    char port[PORT_SIZE+1];
}server;

// takes in a file descriptor
// writes to IP[] and port[]
// returns number of server read from the file ( based on \n )
int server_info(FILE *file,server* s);
int writeit(int fd,char* buff,int size);
int readit(int fd,char* buff,int size);
#endif
