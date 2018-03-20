#ifndef _UTILS_H_INCLUDE_
#define _UTILS_H_INCLUDE_
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define BUFF_SIZE 1024
/* returns the file descriptor */
int bindIpp(char* ip,int port, struct sockaddr_in* addr);
/* makes connection */
void conn(int sockfd,struct sockaddr_in* addr);
/* if size > buff_size => chuncks = buff_size/size */
/* returns num bytes send */
off_t sendFile(int fd,char* buff,off_t size);
/* returns num bytes read */
off_t recvFile(int sockfd,char* buff,int fd);
#endif
