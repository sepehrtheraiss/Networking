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
#define BUFFCHR 4
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
/* protocol */
enum protocol {
    START, CON, STOP,
    RETRY, FAILED, END
};

static char errors[4][30]={
    "payload size exceed",
    "read error",
    "write error",
    "service not implemented"
};

struct host;

/* returns new host, nil on err */
struct host* initHost(char* ip, char* port, uint8_t proto);

/* binds scoket */
bool initSock(struct host* h);

/* closes all remote hosts */
void closeRmtHost(struct host* h);

/* closes socket and frees host */
void closeHost(struct host* h);

/* returns forked pid */
pid_t acceptSession(struct host* src, struct host** dst);

/* formats and sends pakcet */
bool sendMSG(struct host* dst, uint16_t id, uint8_t state, uint32_t size, void* payload);

/* stripes header
 * sets id and state
 * returns size of buffer
 * -1 on error
 * 0 indiciates end of packet
 */
int readMSG(struct host* dst,uint32_t* id, uint8_t* state, void* payload);

#endif
