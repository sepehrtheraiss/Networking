//-------------------------------------------------
// Sepehr Raissian
// Sraissia@ucsc.edu
// CMPE156 
// header.h
//-------------------------------------------------

#ifndef _HEADER_H_INCLUDE_
#define _HEADER_H_INCLUDE_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <arpa/inet.h>
#define SIX_C_SOURCE >= 200809L
#define LISTEN 5
#define BUFF_SIZE 4096
#define HTTP_PORT 80
struct logInfo{
	char date[512];
	char c_ip[32];
	char request[1024]; // includes status code
	int size; // total size of the package
};
char* f_sites[30];
char forward_header[512];
char HOST[256];
// returns the number of times character c has occured in string
int cinStr(char c,char* str,int len);

// splits the string using c 
// returns 1 on succes and 0 on fail
int splitString(char* c,char* str,char** buffer);
// removes this char from string
void stripR(char** lines,int len);

 // 0 on invalid
// 1 on success
int getHostPath(char* host,char* path);

// takes in a lines and tries to find the first occurance of HEAD or GET 
// finds first occurace of Host: after HAED or GET
// find the first occurance of Connection after Host: then overwrites lines from first header request to connection
// if finding was success index of req host and connection are stored
// else -1 is returned
void wrapReq(char** lines,int lines_len,int* indices);

// main function for exchanging packets between server and client 
int exectute(int s_sockfd,int clisockfd,struct sockaddr_in cli_addr,struct sockaddr_in serv_addr);
// returns 1 on site is forbbiden else 0
int isFobidden(char* str);
void Error405(int fd);
int typeReq(char* str);
// establish connection with the given 3 args, head,host,connection
// if connection is to close returns 0 else keep it alive returns 1
// actual exchange function
int fetch_response(int sockfd,char** lines,char* host,int lines_len,int clisockfd);
int getHeaderSize(char* str);
// returns 0 on connection close and 1 on keep it alive
// returns -1 on error
int getConnection(char* str);
int isStr(char* str,char* c);
char* findStr(char* str,char* c);
#endif
