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

int stateInt(char* str){
        if (!strcmp(str,"START"))
            return START;
        if (!strcmp(str,"CON"))
            return CON;
        if (!strcmp(str,"STOP"))
            return STOP;
        if (!strcmp(str,"RETRY"))
            return RETRY;
        if (!strcmp(str,"FAILED"))
            return FAILED;
        if (!strcmp(str,"END"))
            return END;

        return -1;
}
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
bool sendMSG(struct host* dst, uint16_t id, int state, size_t size, void* payload)
{
    char headerSize[BUFFSIZE]={0};
    char header[BUFFSIZE]={0};

    int n = snprintf(header, BUFFSIZE, "%i:%i:%zu", id, state, size);  
    if (n >= BUFFSIZE){
        fprintf(stderr, "header size overflow");
        return false;
    }

    snprintf(headerSize, BUFFSIZE, "%i", n);
    //fprintf(stderr,"headerSize: %s header: %s\n", headerSize, header);
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

/* 
 * stripes header
 * sets id and state
 * returns size of buffer 
 */
int readMSG(struct host* dst, uint16_t* id, int* state, void* payload)
{

    char headerSizeStr[BUFFSIZE]={0};
    int n;

    if((n=read(dst->sockfd,headerSizeStr,BUFFSIZE)) < 0)
    {
        perror("read packet: ");   
        return -1;
    }
    
    size_t headerSize = (size_t)strtol(headerSizeStr, (char**)NULL, 10);
    // need null terminator for strsep
    char header[headerSize+1];
    memset(header,0,headerSize+1);
    if((n=read(dst->sockfd,header,headerSize)) < 0)
    {
        perror("read packet: ");   
        return -1;
    }

    char *token, *string, *tofree;
    char* buffer[3];
    int i = 0;

    tofree = string = strdup(header);
    assert(string != NULL);

    while ((token = strsep(&string, ":")) != nil){
        buffer[i++] = token;
    }

    *state = atoi(buffer[1]);
    free(tofree);

    //fprintf(stderr,"id: %s state: %s size: %s\n",buffer[0],buffer[1],buffer[2]);

    n = 0;
    if(*state != END){
        if((n=read(dst->sockfd,payload,atoi(buffer[2]))) < 0)
        {
            perror("read packet: ");   
            return -1;
        }
    }
    return n;
}


