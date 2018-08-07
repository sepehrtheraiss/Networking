#include "../include/ntutils.h"
/*
struct host{
    uint32_t IPv4;
    uint16_t port;
    uint8_t proto;
    int sockfd;
    struct sockaddr_in addr;
    char* IPv4Str;
    char* portStr;
};
*/
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
            default:
                fprintf(stderr,"proto undefined %i\n",h->proto);
                return false;
        }
        return true;

}
void closeRmtHost(struct host* h)
{
    for(int i =0;i<BACKLOG;i++)
    {
        close(h->sockfd);
    }
}
/* returns forked pid */
pid_t acceptSession(struct host* src,struct host* dst)
{
    socklen_t clilen = sizeof(dst->addr);
    dst->sockfd = accept(src->sockfd, (struct sockaddr *)&dst->addr, &clilen);
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
/* sends pakcet, if tcp bool on returns error msg on error else udp return nil */
char* sendMSG(struct host* dst,void* payload,unsigned int size)
{
    /* 4 bytes + /0 */
    char hex[11]={0};   
    char packetSize[11]={0};
    snprintf(hex,11,"0x%X",size);
    /* left over zeros */
    memset(packetSize,'0',10-strlen(hex)); 
    /* offset after extra zeros 0x0 */
    memcpy(packetSize+(10-strlen(hex)),hex,strlen(hex));

    //fprintf(stderr,"debug: sendMSG: hex: %s dec: %d \n",hex,size);

    if(write(dst->sockfd,packetSize,sizeof(packetSize))<0)
    {
        perror("sendMSG write: ");
    }
    //puts(payload);
    if(write(dst->sockfd,payload,size)<0)
    {
        perror("sendMSG write: ");
    }
    return nil;
}
/* reads pakcet, if tcp bool on returns error msg on error else udp return nil */
char* readMSG(struct host* dst,void** payload)
{
    char buffer[11];
    if(read(dst->sockfd,buffer,11)<0)
    {
        perror("read payload size: ");
    }
    char* hex = strchr(buffer,'x');
    unsigned int payloadSize = (unsigned int)strtol(hex+1, (char **)NULL, 16);
    //fprintf(stderr,"debug: readMSG: payload size hex: %s dec:%u\n",hex,payloadSize);
    *payload = malloc(sizeof(char)*payloadSize);
    if(read(dst->sockfd,*payload,payloadSize)<0)
    {
        perror("read payload: ");   
    }
    return nil;
}
