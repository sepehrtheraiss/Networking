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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <arpa/inet.h>
#define _GNU_SOURCE
#define LISTEN 5
#define BUFF_SIZE 4096
#define HTTP_PORT 80
struct logInfo{
	char date[512];
	char c_ip[32];
	char request[1024]; // includes status code
	int size; // total size of the package
};
// returns the number of times character c has occured in string
int cinStr(char c,char* str,int len);

// splits the string using c 
// returns 1 on succes and 0 on fail
int splitString(char* c,char* str,char** buffer);
// removes this char from string
void chrRemove(char** lines,int len,char c);
#endif
