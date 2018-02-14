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
#include <math.h>
#include <pthread.h>
#define IP_SIZE 16
#define PORT_SIZE 4
#define BUFF_SIZE 512
typedef struct server{
    int id;
    uint32_t IP;
    int port;
    char* str;
}server;

// global variables
char filename[32];
unsigned int fileSize;
unsigned int up; // servers up
/*
int servs_req;
unsigned int sup; // servers up
// total number of servers found in the server-info.txt
// it will be decremented. Once it reaches zero threads can continue
unsigned int t_servers; 
*/
pthread_cond_t lock;
pthread_mutex_t m;
// takes in a file descriptor
// writes to IP[] and port[]
// returns number of server read from the file ( based on \n )
// used by client
int server_info(FILE* file,server* s);
// gets a file size from the first server that is up
// num of servers and number of servers requested
// returns 0 on no servers up
// used by client
int getFileSize(server* servers,char* filename,int num_servs,int servs_req);
// seeks the file then reads n bytes to buffer
// used by server
void read_offset(FILE* file,int off, int bytes,char* buffer);
// writes the offset request to the given server and reads n bytes 
void sendOffsetRead(server* s,int off,int bytes,char* buffer,char* filename);

// returns 1 if server is up else 0
int isUp(server* s);
// will be used for creating a new thread.
// each thread will first check for connectivity and for each connection it increments the sup
// then will read file from the given server
void* initThread(server* s);
int writeit(int fd,char* buff,int size);
int readit(int fd,char* buff,int size);
#endif
