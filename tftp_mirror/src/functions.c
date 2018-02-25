#define _GNU_SOURCE
#include "../include/header.h"
 
void newPort(unsigned int* port,int sockfd,struct sockaddr_in* servaddr)
{
    bzero(servaddr,sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr->sin_port = *port;
    while(bind(sockfd,(struct sockaddr*)servaddr,sizeof(*servaddr)) != 0) // bind failed
    {
        (*port)++;
        servaddr->sin_port = *port;
    }
}

int getFileSize(char* file,int sockfd,struct sockaddr* serv_addr,socklen_t servlen)
{
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set readfds;

    int n;
    char sendline[BUFF_SIZE];
    char recvline[BUFF_SIZE];
    socklen_t len;
    struct sockaddr* reply_addr = malloc(servlen);
    FD_ZERO(&readfds);
    int exit = 0;
    strcpy(sendline,file);
    while(exit != 1)
    {
        FD_SET(sockfd,&readfds);
        sendto(sockfd,sendline,strlen(sendline),0,serv_addr,servlen);
        len = servlen;
        select(sockfd+1,&readfds,NULL,NULL,&timeout);
        if(FD_ISSET(sockfd,&readfds))
        {
            n = recvfrom(sockfd,recvline,BUFF_SIZE,0,reply_addr,&len);
            recvline[n] = 0;
            if(parse(recvline,n) == 0)
            {
                exit = 1;
            }
        }
        else
        {
            fprintf(stderr,"timeout\n");
            exit = 1;
        }
    }
    free(reply_addr);
    return atoi(recvline);

}
// returns index of parses, for now just single index later multiple
int p_num(char* str,int len)
{
    int i =0;
    while(str[i] != ',' && i < len)
    {
        i++;
    }
    return i;
}
int p_offset(char* str,char* filename,uint32_t* offset,uint32_t* bytes)
{
    char off[32];
    char byte[32];
    int i =0;
    int j=0;//where left off
    while(str[++i] != ':');
    memcpy(filename,str,i);
    filename[i] = 0;
    j = i+1;
    //printf("%s\n",filename);
    while(str[++i]!=',');//offset
    memcpy(off,str+j,i-j);
    off[i-j] = 0;
    *offset = atoi(off);
    //printf("%i\n",*offset);
    j = i +1;
    while(str[++i]!=0);
    memcpy(byte,str+j,i-j);
    byte[i-j] = 0;
    *bytes = atoi(byte);
    //printf("%i\n",*bytes);
    return 1;
}
int parse(char* str,int len)
{
    char op;
    int st_code = -1;
    char buff[BUFF_SIZE];
    int last_index = 0;
    if(str != NULL)
    {
        if(len <= BUFF_SIZE && str[0] == '{')
        {
            op = str[1];
            if(op == '2')
            {
                return 2;
            }

                last_index = 0;
                // find the last index of '}'
                while(str[++last_index]!='}' && str[last_index+1] != '$' && last_index < BUFF_SIZE -1);
                if(last_index > BUFF_SIZE - 1) // need the last one for \0
                {
                    return st_code;
                }

            // actually dont need this, but just incase if I need to add something later
            switch(op)
            {
                case '0':
                    st_code = 0;
                    break;
                case '1':
                    st_code = 1;
                    break;
                case '2':
                    st_code = 2;
                    break;
                default:
                    st_code = -1;
            }
            memcpy(buff,str+3,last_index-3);
            buff[last_index-3] = 0;
            //str = buff; why doesnt it work ?
            strcpy(str,buff);
            //write(1,buff,last_index-2);

        }// [1] is { and size <= 512
    }//if no null
    return st_code;
}

void 
dg_cli(FILE* fp,int sockfd,struct sockaddr* serv_addr,socklen_t servlen)
{
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set readfds;

    int n;
    char sendline[BUFF_SIZE];
    char recvline[BUFF_SIZE];
    char buff[BUFF_SIZE];
    socklen_t len;
    struct sockaddr* reply_addr = malloc(servlen);
    FD_ZERO(&readfds);
    int exit = 0;
    int p_index = 0; // , index
    // get file size and port to listen on
    strcpy(sendline,"{1:taxes.txt}$");
    while(exit != 1)
    {
        FD_SET(sockfd,&readfds);
        sendto(sockfd,sendline,strlen(sendline),0,serv_addr,servlen);
        len = servlen;
        select(sockfd+1,&readfds,NULL,NULL,&timeout);
        if(FD_ISSET(sockfd,&readfds))
        {
            n = recvfrom(sockfd,recvline,BUFF_SIZE,0,reply_addr,&len);
            recvline[n] = 0;
            p_index = p_num(recvline,n); 
            memcpy(buff,recvline,p_index);
            printf("msg: %s\n",buff);
            memcpy(buff,recvline+p_index+1,n);
            printf("msg: %s\n",buff);
            exit = 1;
        }
        else
        {
            printf("timeout\n");
        }
    }
    free(reply_addr);
}
// server
void dg_echo(int sockfd, struct sockaddr* serv_addr,socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[BUFF_SIZE];

    for(;;)
    {
        len = clilen;
        n = recvfrom(sockfd,msg,BUFF_SIZE,0,serv_addr,&len);
        printf("msg: %s\n",msg);
        strcpy(msg,"{2:42567}$,{3:6969}$");
        sendto(sockfd,msg,BUFF_SIZE/4,0,serv_addr,len);
    }
}

// takes in a file descriptor
// converts string to unit32_t and int 
// writes to IP[] and port[]
// returns number of server read from the file ( based on \n )
int server_info(FILE *file,struct server* s)
{
    int n = 0; // num serves
    char *line = NULL;
    char IP[IP_SIZE+1];
    char port[PORT_SIZE+1];
    size_t linecap = 0;
    ssize_t linelen;
    int i;
    int j;
    while ((linelen = getline(&line, &linecap, file)) > 0)
    {
        i = 0;
        // IP
        while((int)line[i]!= 32){++i;}
        memcpy(&IP[0],&line[0],i);
        IP[i] = 0;
        if(inet_pton(AF_INET,&IP[0],&(s[n].IP))<1){
             perror("ERROR pasring IP address");
            exit(EXIT_FAILURE);
        } 
        j=i;
        while(line[i]!='\n'){++i;}
        memcpy(&port[0],&line[j+1],i-j-1);
        port[i-j] = 0;
        s[n].port = htons(atoi(&port[0]));
        n++; // num servers
    }
    return n;
}

uint32_t read_offset(FILE *file,uint32_t off, uint32_t bytes,char *buffer)
{
   fseek(file,off,SEEK_SET);
   int n = fread(buffer,1,bytes,file);
   buffer[n]=0;
   return n;
}
void sendOffsetRead(server* s,uint32_t off,uint32_t bytes,char* buffer,char* filename)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    int buff_read;
    
           /* Create a socket point */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
       
        if (sockfd < 0) {
          perror("ERROR opening socket");
          exit(EXIT_FAILURE);
        }
        //make connection
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = s->IP;
        serv_addr.sin_port = s->port;
       
        /* Now connect to the server */
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
        }
        else
        {
            sprintf(buffer,"offset %s(%i,%i)",filename,off,bytes);
            printf("%s\n",buffer);
            write(sockfd,buffer,BUFF_SIZE);
            buff_read = read(sockfd,buffer,bytes);
            buffer[buff_read] = 0;
            write(1,buffer,buff_read);
        }//end else made a connections
        if(close(sockfd) != 0){
            perror("ERROR on close sockfd");
            exit(EXIT_FAILURE);
        }//end close socket


}
int send_wait(char* msg,int sockfd,struct sockaddr* serv_addr,socklen_t servlen)
{
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set readfds;

    int n;
    char sendline[BUFF_SIZE];
    char recvline[BUFF_SIZE];
    char buff[BUFF_SIZE];
    socklen_t len;
    struct sockaddr* reply_addr = malloc(servlen);
    FD_ZERO(&readfds);
    int exit = 0;
    // get file size and port to listen on
    strcpy(sendline,msg);
    FD_SET(sockfd,&readfds);
    sendto(sockfd,sendline,strlen(sendline),0,serv_addr,servlen);
    len = servlen;
    select(sockfd+1,&readfds,NULL,NULL,&timeout);
    if(FD_ISSET(sockfd,&readfds))
    {
        n = recvfrom(sockfd,recvline,BUFF_SIZE,0,reply_addr,&len);
        recvline[n] = 0;
        printf("recv msg:%s\n",recvline);
        exit = 1;
    }
    else
    {
        printf("timeout\n");
    }
    
    free(reply_addr);
    return exit;
}
int sendMSG(server* s,char* msg)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int up = 0;
       
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
        //make connection
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = s->IP;
        serv_addr.sin_port = s->port;
        //printf("ip:%i port:%i\n",s->IP,s->port);
        /* Now connect to the server */
                        // just to make the msg look nice
        up = send_wait(msg,sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        if(close(sockfd) != 0){
            perror("ERROR on close sockfd");
            exit(EXIT_FAILURE);
        }
    return up;
}
int isUp(server* s)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int up = 0;
       
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
        //make connection
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = s->IP;
        serv_addr.sin_port = s->port;
        //printf("ip:%i port:%i\n",s->IP,s->port);
        /* Now connect to the server */
                        // just to make the msg look nice
        up = send_wait("{9:you up?}$",sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        if(close(sockfd) != 0){
            perror("ERROR on close sockfd");
            exit(EXIT_FAILURE);
        }//end close socket
    return up;

}

// returns 1 if bytes read is equal to chunk size
//int getFileChunk(server* s,int* sockfd,fd_set* readfds,struct sockaddr_in* serv_addr,struct sockaddr* reply_addr,socklen_t* len)
int getFileChunk(server* s)
{
    char sendline[BUFF_SIZE];
    char recvline[BUFF_SIZE];
    char buff[BUFF_SIZE];
    char t_buff[16];
    uint32_t offset;
    uint32_t bytes;
    uint16_t FRAG_SIZE = ceil((double)file_size /chunck); 
    printf("s.id: %i\n",s->id);
    uint16_t seek = s->id*FRAG_SIZE;
    sprintf(buff,"{1:%s:%i,%i}$",filename,seek,FRAG_SIZE);
    strcpy(sendline,buff);
    uint8_t exit = 0;
    int n;
    complete = 0;
    printf("sendline: %s\n",sendline);
    if(sendMSG(s,sendline)==1) // good to go
    {
        printf("good to go\n");
    }
    /*
    if(sendto(*sockfd,sendline,strlen(sendline),0,(struct sockaddr *)serv_addr,sizeof(serv_addr))<1)
    {
        perror("error on sendto");
    }
    

    while(exit != 1)
    {
        
        if(FD_ISSET(*sockfd,readfds))
        {
            n = recvfrom(*sockfd,recvline,BUFF_SIZE,0,reply_addr,len);
            recvline[n] = 0;
            printf("status code:%i",parse(recvline,n));
            strcpy(t_buff,filename);
            p_offset(recvline,t_buff,&offset,&bytes);
            if(q_exist(s->q,filename,offset,bytes))
            {
                q_insert(s->q,recvline,offset,bytes);
            }
            printf("%s\n",recvline);
            if(q_bytesRead(s->q) == FRAG_SIZE)
            {
                complete = 1;
                return 1;
            }
        }   
        else
        {
            printf("timeout\n");
            exit = 1;
            complete = 0;
        }

    }*/
    return 0;

}
void* initThread(server* s)
{
    // int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    // struct sockaddr_in serv_addr;
    // bzero((char *) &serv_addr, sizeof(serv_addr));
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = s->IP;
    // serv_addr.sin_port = s->port;

    // unsigned int* port = 5000 + (50 * s[i].id)
    // newPort(port,sockfd,serv_addr);

    // struct timeval timeout;
    // timeout.tv_sec  = 1;
    // timeout.tv_usec = 0;
    // fd_set readfds;
    // socklen_t len;
    // struct sockaddr* reply_addr = malloc(sizeof(serv_addr));
    // FD_ZERO(&readfds);
    int exit = 0;
    // get file size and port to listen on
    // FD_SET(sockfd,&readfds);
    // len = sizeof(serv_addr);
    // select(sockfd+1,&readfds,NULL,NULL,&timeout);
    // create a function here for stuff below
    int counter = 0;
    while(exit != 1 && counter != 2) // try it 10 times first 
    {
        counter++;
        exit = getFileChunk(s);
    }
   // free(reply_addr);
    //close(sockfd);
    up--;
    return NULL;
}


