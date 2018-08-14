#include "../include/ntutils.h"

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

/* closes all remote hosts */
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

/* formats and sends pakcet */
char* sendMSG(struct host* dst,void* payload,unsigned int size)
{
    if(size > MAXHEX)
    {
        return errors[0];        
    }
    /* 32 bytes + 0x + /0 */
    char hex[HEXSIZE+3]={0};   
    char header[HEXSIZE+3]={0};
    snprintf(hex,HEXSIZE,"0x%X",size);
    /* left over zeros prepend 0x */
    memset(header,'0',(HEXSIZE-2)-strlen(hex)); 
    /* offset after extra zeros. 0xA append 000 */
    memcpy(header+((HEXSIZE-2)-strlen(hex)),hex,strlen(hex));

    //fprintf(stderr,"debug: sendMSG: hex: %s dec: %d \n",hex,size);

    if(write(dst->sockfd,header,sizeof(header))<0)
    {
        perror("sendMSG write: ");
        return errors[2];
    }
    //puts(payload);
    if(write(dst->sockfd,payload,size)<0)
    {
        perror("sendMSG write: ");
        return errors[2];
    }
    return nil;
}

/* dynamically allocates buffer for payload */
char* readMSG(struct host* dst,void** payload)
{
    /* 32 bytes + 0x + /0 */
    char buffer[HEXSIZE+3];
    if(read(dst->sockfd,buffer,HEXSIZE+3)<0)
    {
        perror("read payload size: ");
        return errors[1];
    }
    char* hex = strchr(buffer,'x');
    if(*(hex-1) != '0')
    {
       return errors[3]; 
    }
    unsigned int payloadSize = (unsigned int)strtol(hex+1, (char **)NULL, 16);
    //fprintf(stderr,"debug: readMSG: payload size hex: %s dec:%u\n",hex,payloadSize);
    *payload = malloc(sizeof(char)*payloadSize);
    if(read(dst->sockfd,*payload,payloadSize)<0)
    {
        perror("read payload: ");   
        return errors[1];
    }
    return nil;
}
