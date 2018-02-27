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
    FD_CLR(sockfd,&readfds);
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
int p_offset(char* str,char* filename,int* offset,int* bytes)
{
//    write(1,str,BUFF_SIZE);
    char off[32];
    char byte[32];
    int i =0;
    int j=0;//where left off
    while(str[++i] != ':');//:filename
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
    while(str[++i]!=':');
    memcpy(byte,str+j,i-j);
    byte[i-j] = 0;
    *bytes = atoi(byte);
    //printf("%i\n",*bytes);
    //printf("buff [%c %c %c]\n",str[i-1],str[i],str[i+1]);
    return i; // returns the index of : after bytes {2:filename:i,n:string
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

int read_offset(FILE *file,int off, int bytes,char *buffer)
{
   fseek(file,off,SEEK_SET);
   int n = fread(buffer,1,bytes,file);
   buffer[n]=0;
   return n;
}
int revc_wait(int sockfd,char* msg,char* buff)
{
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set readfds;
    int exit_code = 0;
    int n;
    char recvline[BUFF_SIZE];
    FD_SET(sockfd,&readfds);
    select(sockfd+1,&readfds,NULL,NULL,&timeout);
    if(FD_ISSET(sockfd,&readfds))
    {
        n = recvfrom(sockfd,recvline,BUFF_SIZE,0,NULL,NULL);//reply_addr,&len);
        recvline[n] = 0;
        //write(1,recvline,n);
        if(buff != NULL)
        {
            //strcpy(buff,recvline);
            memcpy(buff,&recvline[3],n);
            //write(1,buff,n);
            char beef[BUFF_SIZE];
            int off,bytes;
            int i = p_offset(buff,beef,&off,&bytes);
            //printf("filename: %s off:%i bytes:%i\n",beef,off,bytes);
            //write(1,buff,n)
           // printf("%s",buff);
        }
      //  printf("recv msg:%s\n",recvline);
        exit_code = 1;
    }
    else
    {
        printf("timeout\n");
    }
    FD_CLR(sockfd,&readfds);
    return exit_code;
}
int send_wait(char* msg,int sockfd,struct sockaddr* serv_addr,socklen_t servlen,char* buff)
{
    char sendline[BUFF_SIZE];
    strcpy(sendline,msg);
    sendto(sockfd,sendline,strlen(sendline),0,serv_addr,servlen);
    return revc_wait(sockfd,msg,buff);
}
int sendMSG(server* s,char* msg,char* buff)
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
        up = send_wait(msg,sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr),buff);
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
        up = send_wait("{9:you up?}$",sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr),NULL);
        if(close(sockfd) != 0){
            perror("ERROR on close sockfd");
            exit(EXIT_FAILURE);
        }//end close socket
    return up;

}
int recv_inChunks(server* s,int index)
{

    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int up = 0;
       
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
/*
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    fd_set readfds;
*/
    int n;
    int offset,bytes;
    char sendline[BUFF_SIZE];
    char recvline[BUFF_SIZE+1];
    char buff[BUFF_SIZE+1];
    socklen_t len;
    //struct sockaddr* reply_addr = malloc(sizeof(serv_addr));
  //  FD_ZERO(&readfds);
    int stop = 0;
  
  /*  //FD_SET(sockfd,&readfds);
    printf("fdread: %p\n",&readfds);
    if(select(sockfd+1,&readfds,NULL,NULL,&timeout)<0)
    {
        perror("error on select");
        exit(1);
    }

    int re_try = 0;
    while(re_try < 3 && stop != 1)
    {
        if((stop=revc_wait(sockfd) == 1)){
            printf("recv on file chunk good\n");
        }
        else
        {
            printf("recv on file chunk failed\n");   
        }
        re_try++;
    }
   
    int string_index = 0;*/
    /*
    while(stop != 1)
    {
            if(FD_ISSET(sockfd,&readfds))
            {
                n = recvfrom(sockfd,recvline,BUFF_SIZE,0,(struct sockaddr*)&serv_addr,&len);
                recvline[n] = 0;
                printf("status code:%i",parse(recvline,n));
                strcpy(buff,filename);
                string_index = p_offset(recvline,buff,&offset,&bytes) + 1;
                memcpy(buff,recvline+string_index,bytes);
                buff[bytes] = 0;
                printf("%s",buff);
                if(q_exist(s->q[index],filename,offset,bytes))
                {
                    q_insert(s->q[index],recvline,offset,bytes);
                }
                if(q_bytesRead(s->q[index]) == s->q[index]->off_bytes[1])
                {
                    complete = 1;
                    return 1;
                }
                stop = 1;
            }   
            else
            {
                printf("timeout in recv_inChunks\n");
                stop = 1;
                complete = 0;
            }

    }//end while
      //  free(reply_addr);
        FD_CLR(sockfd,&readfds);
        */
        if(close(sockfd) != 0){
            perror("ERROR on close sockfd");
            exit(EXIT_FAILURE);
        }//end close socket
        printf("end of recv_inChunks functions\n");
        return 0;
}
// returns 1 if bytes read is equal to chunk size
//int getFileChunk(server* s,int* sockfd,fd_set* readfds,struct sockaddr_in* serv_addr,struct sockaddr* reply_addr,socklen_t* len)
int getFileChunk(server* s,int index)
{
    char sendline[BUFF_SIZE];
    char recvline[BUFF_SIZE];
    char buff[BUFF_SIZE+1]; //=malloc(sizeof(char)*BUFF_SIZE+1);
    char str[BUFF_SIZE+1];
    char t_buff[16];
    int offset;
    int bytes;
    //printf("index:%i\n",index);
    sprintf(buff,"{1:%s:%i,%i:}$",filename,s->q[index]->off_bytes[0],s->q[index]->off_bytes[1]);
    strcpy(sendline,buff);
    int exit = 0;
    int n;
    complete = 0;
    printf("sendline: %s\n",sendline);
    if(sendMSG(s,sendline,buff)==1) // good to go
    {
        printf("good to go\n");
    }
    int s_i = p_offset(buff,t_buff,&offset,&bytes);
    //write(1,&buff[s_i+1],bytes);
    //write(1,buff,bytes);
    memcpy(str,&buff[s_i+1],bytes);
    //printf("%s\n",buff+s_i);
    //write(1,str,bytes);
    str[bytes]= 0;
    //printf("buff: %i",s_i);
    if(q_exist(s->q[index],str,offset,bytes) != 1)
    {
        q_insert(s->q[index],str,offset,bytes);
    }
    return 1;//recv_inChunks(s,index);
}
void* initThread(server* s)
{

    int exit = 0;
    int retry = 0;
    while(exit != 1 && retry != 1) // try it x times before exiting 
    {
        retry++;
        exit = getFileChunk(s,s->counter++);
    }
    up--;
    printf("end initThread\n");
    return NULL;
}


