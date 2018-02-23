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
#include <sys/socket.h>
#include "queue.h"
#define IP_SIZE 16
#define PORT_SIZE 4
#define BUFF_SIZE 512
typedef struct server{
    int id;
    uint32_t IP;
    int port;
    queue* q;
}server;

// global variables
char filename[128];
uint32_t file_size;
unsigned int up; // servers up
unsigned int SUP; // servers up will not be modified

// tries to bind the given port if no good then incs it
void newPort(unsigned int* port,int sockfd,struct sockaddr_in* servaddr);
// returns the port to request the file, the sets file to file_size
int getFileSize(char* file,int sockfd,struct sockaddr* serv_addr,socklen_t servlen);
// returns index of parses, for now just single index later multiple
int p_num(char* str,int len);
// used to parse file name and offset return 1 on success
int p_offset(char* str,char* filename,int* offset,int* bytes);
// parses string, points str to the new buffer then returns the corresponding int operation
// 0 data_size
// 1 file_size 
// 2 filename,offset
// -1 on error
int parse(char* str,int len);
//client
void dg_cli(FILE* fp,int sockfd,struct sockaddr* serv_addr,socklen_t servlen);
// server
void dg_echo(int sockfd, struct sockaddr* serv_addr,socklen_t clilen);
// takes in a file descriptor
// writes to IP[] and port[]
// returns number of server read from the file ( based on \n )
// used by client
int server_info(FILE* file,server* s);
// gets a file size from the first server that is up
// num of servers and number of servers requested
// returns 0 on no servers up
// used by client
//int getFileSize(server* servers,char* filename,int num_servs,int servs_req);
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
