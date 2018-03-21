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
#include <signal.h>

#define BUFF_SIZE 1024
#define MAX_ARGS_LEN 32
/* 
 * CMD:CMDD(4+4)
 * :args (1+32)
 * 0:+1 
 */
#define MAX_CMD_LEN 42
#define LIST 0
#define RETR 1
#define STOR 2
#define QUIT 3
/* returns the file descriptor */
int bindIpp(char* ip,int port, struct sockaddr_in* addr,int mode);
/* makes connection
 * returns -1 on fail
 */
int conn(int sockfd,struct sockaddr_in* addr);
/* if size > buff_size => chuncks = buff_size/size */
/* returns num bytes send */
off_t sendFile(int sockfd,char* buff,off_t size);
/* returns num bytes read */
off_t recvFile(int sockfd,char* buff,int fd);
/* 
 * formats the command with args then sends it
 * if it is a retrivial type of command it will fork then open the data port
 * if there was an issue on the server side a reply from the server will kill the child
 */
int sendCMD(int sockfd,char* cmdBuff,char* args,struct sockaddr_in* cli_addr);
/* 
 * formats the command with the recvived args then checks for error checking
 * if it is a retrivial type of command it will fork then open the data port
 * if there was an issue on the server side a reply from the server will kill the child
 */
int recvCMD(int sockfd,struct sockaddr_in* cli_addr);

/* Helper functions */

/*
 * checks for error validation then returns the status code. Checks in this order:
 * 1. 500: syntax cmdBuff
 * 2. 501: syntax args
 * 3. 425: can't open data port
 * 4. 200: ok
 * 5. 452: Error writing a file is special 
 */
int errorCode(char* cmdBuff,char* args,struct sockaddr_in* addr);
/* inserts the actual cmdBuff into cmdBuff */
void strCMD(char* cmdBuff);
/* returns the corresponding macro for cmdBuff */
int wCMD(char* cmdBuff);
int splitString(char* c,char* str,char** buffer);
void freeTokens(char** buffer,int ntoks);
void errorCheck(int cond, char* errmsg, int mode);
#endif
