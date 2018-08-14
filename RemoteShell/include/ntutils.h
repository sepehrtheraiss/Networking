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
#define HEXSIZE 8
#define MAXHEX 0xFFFFFFFF
#define BACKLOG 1
#define MAXCLI  5
#define TCP 0
#define UDP 1
#define CLITCP 3
typedef int bool;
#define true 1
#define false 0

static char errors[4][30]={
    "payload size exceed",
    "read error",
    "write error",
    "service not implemented"
};
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
/* formats and sends pakcet */
char* sendMSG(struct host* dst,void* payload,unsigned int size);
/* dynamically allocates buffer for payload */
char* readMSG(struct host* dst,void** payload);

#endif
