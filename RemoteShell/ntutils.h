#ifndef __HEADER__NTUTILS__
#define __HEADER__NTUTILS__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nil NULL
#define BUFFSIZE 256
#define BACKLOG 1
#define TCP 0
#define UDP 1
#define CLITCP 3
typedef int bool;
#define true 1
#define false 0

//struct host;

struct host{
    uint32_t IPv4;
    uint16_t port;
    uint8_t proto;
    int sockfd;
    struct sockaddr_in addr;
    char* IPv4Str;
    char* portStr;
};

/* binds scoket */
bool initSock(struct host* h);
/* closes all remote hosts */
void closeRmtHost(struct host* h);
/* returns forked pid */
pid_t acceptSession(struct host* src, struct host* dst);
/* sends pakcet, if tcp bool on returns error msg else udp return nil */
char* sendMSG(struct host* dst,void* payload,unsigned int size);
char* readMSG(struct host* dst,void* payload);
/* */
#endif
