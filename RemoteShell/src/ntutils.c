#include "../include/ntutils.h"

struct host{
    uint32_t IPv4;
    uint16_t port;
    uint8_t proto : 4;
    int sockfd;
    struct sockaddr_in addr;
    char* IPv4Str;
    char* portStr;
    uint8_t state;
};

/* returns new host, nil on err */
struct host* initHost(char* ip, char* port, uint8_t proto)
{
    struct host* h = malloc(sizeof(struct host));

    h->IPv4Str = ip;
    h->portStr = port;
    h->proto = proto;

    if (!initSock(h))
    {
        fprintf(stderr,"error: initSock()\n");
        return nil;
    }

    listen(h->sockfd,BACKLOG);
    return h;
}
/* binds scoket return 0 on fail and 1 on success */
bool initSock(struct host* h)
{
        switch(h->proto)
        {
            case TCP:
                h->sockfd = socket(AF_INET, SOCK_STREAM,0);
                break;
            case CLITCP:
                h->sockfd = socket(AF_INET, SOCK_STREAM,0);
                break;
            case UDP:
                h->sockfd = socket(AF_INET, SOCK_DGRAM,0);
                break;
            default:
                fprintf(stderr,"[proto invalid]\n");
        }

        if(h->sockfd < 0){
            perror("error opening socket");
            return false;
        }

        if (!inet_pton(AF_INET,h->IPv4Str,&h->IPv4)){
            perror("inet_pton error");
            return false;
        }
        
        memset(&h->addr,0,sizeof(h->addr));
        h->port = htons((int)strtol(h->portStr, (char **)NULL, 10));
        h->addr.sin_family = AF_INET;
        h->addr.sin_addr.s_addr = h->IPv4;
        h->addr.sin_port = h->port;

        switch(h->proto)
        {
            case TCP:
                if (bind(h->sockfd, (struct sockaddr *) &h->addr, sizeof(h->addr)) < 0) {
                    perror("ERROR on binding");
                    exit(EXIT_FAILURE);
                }
                break;
            case CLITCP:
               if (connect(h->sockfd, (struct sockaddr*)&h->addr, sizeof(h->addr)) < 0) {
                  perror("ERROR connect");
                  exit(EXIT_FAILURE);
               }
                break;
            case UDP:
                break;
            default:
                fprintf(stderr,"proto undefined %i\n",h->proto);
                return false;
        }

        return true;
}

/* closes all remote hosts */
void closeRmtHost(struct host* h)
{
    for(int i =0;i<BACKLOG;i++)
    {
        close(h->sockfd);
    }
}

/* closes socket and frees host */
void closeHost(struct host* h)
{
    close(h->sockfd);
    free(h);
    h = nil;
}

/* returns forked pid */
pid_t acceptSession(struct host* src,struct host** dst)
{
    socklen_t clilen = sizeof((*dst)->addr);
    *dst = malloc(sizeof(struct host));
    (*dst)->sockfd = accept(src->sockfd, (struct sockaddr *)&((*dst)->addr), &clilen);
    pid_t pid = fork();
    if(pid != 0)
    {
       return pid; 
    }
    else
    {
        if(pid == -1){
            perror("error on fork: ");
            exit(EXIT_FAILURE);
        }
        close(src->sockfd);
        return pid;
    }
}

/* formats and sends pakcet */
bool sendMSG(struct host* dst, uint16_t id, uint8_t state, uint32_t size, void* payload)
{
    char headerSize[BUFFSIZE]={0};
    char header[BUFFSIZE]={0};
    char packet[BUFFSIZE]={0};

    int n = snprintf(header, BUFFSIZE, "%i:%i:%i", id, state, size);  
    if (n >= BUFFSIZE){
        fprintf(stderr, "header size overflow");
        return false;
    }

    snprintf(headerSize, BUFFSIZE, "%i", n);
    
    if(write(dst->sockfd,headerSize,BUFFSIZE)<0)
    {
        perror("sendMSG write: ");
        return false;
    }
    if(write(dst->sockfd,header,n)<0)
    {
        perror("sendMSG write: ");
        return false;
    }
    if(write(dst->sockfd,payload,size)<0)
    {
        perror("sendMSG write: ");
        return false;
    }

    return true;
}

/* stripes header
 * sets id and state
 * returns size of buffer */
int readMSG(struct host* dst,uint32_t* id, uint8_t* state, void* payload)
{
    int n;
    if((n=read(dst->sockfd,payload,BUFFSIZE)) < 0)
    {
        perror("read packet: ");   
        return -1;
    }

    char* token;
    //char** buffer = malloc(sizeof(char*)*4);
    char* buffer[4];
    int i =0;
    while((token=strsep((char**)&payload,":"))!= nil) {
        buffer[i++] = token;
    }

    fprintf(stderr,"%s %s %s %s",buffer[0],buffer[1],buffer[2],buffer[3]);
    return n;
}
