//-------------------------------------------------
// Sepehr Raissian
// Sraissia@ucsc.edu
// CMPE156 
// header.h
//-------------------------------------------------

#ifndef _HEADER_H_INCLUDE_
#define _HEADER_H_INCLUDE_
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#define IP_SIZE 16
#define PORT_SIZE 4
#define BUFF_SIZE 128
typedef struct server{
    uint32_t IP;
    int port;
}server;

// takes in a file descriptor
// writes to IP[] and port[]
// returns number of server read from the file ( based on \n )
int server_info(FILE *file,server* s);
// gets a file size from the first server that is up
// num of servers and number of servers requested
// returns 0 on no servers up
int getFileSize(server* servers,char* filename,int num_servs,int servs_req);
// seeks the file then reads n bytes to buffer
void read_offset(FILE *file,int off, int bytes,char *buffer);
int writeit(int fd,char *buff,int size);
int readit(int fd,char *buff,int size);
#endif
