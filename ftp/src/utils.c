#include "../include/utils.h"

int bindIpp(char* ip,int port, struct sockaddr_in* addr,int mode)
{
    /* First call to socket() function returns file descriptor */
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   uint32_t IP;
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
   }
   
   if(mode == 1)
   {
     if (bind(sockfd, (struct sockaddr *) addr, sizeof((*addr))) < 0) {
        perror("ERROR on binding");
        return -1;
     }
   }
   else{

     /* Initialize socket structure */
     bzero((char *) addr, sizeof((*addr)));

     if(inet_pton(AF_INET,ip,&IP)<1){
        perror("ERROR pasring IP address");
        exit(EXIT_FAILURE);
     } 

     /* INADDR_ANY (0.0.0.0) means any address avaiable for binding*/
     addr->sin_family = AF_INET;
     addr->sin_addr.s_addr = IP; 
     addr->sin_port = htons(port);
      if (bind(sockfd, (struct sockaddr *) addr, sizeof((*addr))) < 0) {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
      }

   }


    return sockfd;
}

int conn(int sockfd,struct sockaddr_in* addr)
{
   if (connect(sockfd, (struct sockaddr*)addr, sizeof((*addr))) < 0) {
      perror("ERROR connecting");
      return -1;
   }
   return 0;
}
/* void append() this function will be used for huge files such that it calls sendFile * n times because its too big to fit in memory */

/* if size > buff_size => chuncks = buff_size/size*/
/* buffer will be exactly the sized passed */
off_t sendFile(int sockfd,char* buff,off_t size)
{
    /* take the celing and send n 0's for left overs */
    int chuncks = ceil((double)size / BUFF_SIZE);
    /* if 0 good otherwise positive send n 0's */
    off_t left_over = (chuncks*BUFF_SIZE) - size; 
    printf("read left overs: %lli\n",left_over);
    
    off_t bytes_write = 0;
    int i=0;
    while(i < chuncks)
    {
        bytes_write += BUFF_SIZE; 
        write(sockfd,buff+(i*BUFF_SIZE),BUFF_SIZE);
        i++;
    }

    if(left_over > 0)
    {
        memset(buff,0,left_over);
        write(sockfd,buff,left_over);
    }

    return bytes_write;
}

off_t recvFile(int sockfd,char* buff,int fd)
{

    off_t bytes_read = 0;
    int n = 0;
    while( n > 0)
    {
        /* fd needs to first create and appendable */
        n = read(sockfd,buff,BUFF_SIZE);
        /* write either to a file or stdout  -1 for none to clear the buffer*/
        if(fd != -1)
            write(fd,buff,n);

        bytes_read += n;
    }
        
    return bytes_read;
}
void dealWithServer(char* cmdBuff,char* args,struct sockaddr_in* cli_addr)
{
  struct sockaddr_in data_adrr,serv_addr;
  char* token[4];
    memcpy(&data_adrr,cli_addr,sizeof(data_adrr));
    data_adrr.sin_port += 1;
    int dsockfd;
    dsockfd = bindIpp(0,0,&data_adrr,1);
    int re_try =0;
    while(dsockfd < 0)
    {
      printf("data prot unavailable, re-trying\n");
      sleep(1);
      dsockfd = bindIpp(0,0,&data_adrr,1);
      re_try++;
      if(re_try > 10)
      {
          printf("sorry data port is unavailable please try again later\n");
      }
    }
    listen(dsockfd,1);
    socklen_t servlen = sizeof(serv_addr);
    int ssockfd = accept(dsockfd, (struct sockaddr *)&serv_addr, &servlen);
    char buffer[BUFF_SIZE];
    if(wCMD(cmdBuff)==QUIT)
        exit(1);
    if(wCMD(cmdBuff)==LIST)
    {
      read(ssockfd,buffer,BUFF_SIZE);
      printf("%s",buffer);
    }
    else if(wCMD(cmdBuff)==RETR)
    {
      errorCheck(args == NULL, "sendCMD: args is null\n", 0);
      int fd = open(args,O_TRUNC | O_CREAT | O_WRONLY | O_APPEND);
      recvFile(ssockfd,buffer,dsockfd);
      close(ssockfd);
      close(fd);
    }
    else if(wCMD(cmdBuff)==STOR)
    {
      int fd = open(token[2],O_RDONLY);
      struct stat buf;
      fstat(fd,&buf);
      read(fd,buffer,buf.st_size);
      sendFile(ssockfd,buffer,buf.st_size);
      close(ssockfd);
      close(fd);
    }
    close(dsockfd);
    close(ssockfd);  
}
/* CLEINT
 * sends the command
 * then binds to data port and reads
 * returns the status code
 */
int sendCMD(int sockfd,char* cmdBuff,char* args,struct sockaddr_in* cli_addr)
{

  char buff[MAX_ARGS_LEN];
  memset(buff,0,MAX_ARGS_LEN);
  //int n = snprintf(buff,MAX_CMD_LEN,"CMD:%s:%s",cmdBuff,args);
  int n = sprintf(buff,"CMD:%s:%s",cmdBuff,args);
  pid_t dPort;
  if(n >= MAX_CMD_LEN)
  {
    fprintf(stderr, "sendCMD > MAX_CMD_LEN:%s\n",buff);
    return -1;
  }
  write(sockfd,buff,MAX_CMD_LEN);
  dPort = fork();
  if(dPort != 0)
  {
    read(sockfd,buff,3);
    printf("status code:%s\n",buff);
    if(buff[0] == '2')
    {
      if(buff[1] == '0' && buff[2] == '0')
      {
        printf("200: Command Ok\n");
        return 200;
      }
      
      else
      {
        printf("221: GoodBye\n");
        return 221;
      }
        
    }
    else
    { 
      if(buff[0] == '5')
      {
        if(buff[2]=='0')
        {
          printf("500: Syntax error (unrecognized command)\n");
          return 500;
        }
        else
        {
          printf("501: Syntax error (invalid arguments)\n"); 
        }
      }
      else if(buff[0]=='4')
      {
        if(buff[1]=='2' && buff[2]=='5')
        {
          printf("425: cant open data connection\n");
          return 425;
        }
      }
    }
    /* the idea is for the read to wait until the data transmission is compelete or faield */
    kill(dPort, SIGKILL);
  }
  else if(dPort == 0)
  {
    dealWithServer(cmdBuff,args,cli_addr);
  } 
  return 0;


}
/* SERVER
 * formats the command with the recvived args then checks for error checking
 * if it is a retrivial type of command it will fork then open the data port
 * if there was an issue on the server side a reply from the server will kill the child
 * returns status code
 */
int recvCMD(int sockfd,struct sockaddr_in* cli_addr)
{
  char buff[MAX_CMD_LEN];
  memset(buff,0,MAX_CMD_LEN);
  read(sockfd,buff,MAX_CMD_LEN);
  /* 
   * 0: CMD
   * 1: cmdBuff
   * 2: args
   * 3: NULL
   */
  char* token[4]; 
  int n;
  int nt =splitString(":",buff,token);

  printf("nt:%i\nt1: %s\nt2:%s %s\n",n,token[0],token[1],token[2]);
  /* data port */
  struct sockaddr_in data_adrr;
  memcpy(&data_adrr,cli_addr,sizeof(data_adrr));
  data_adrr.sin_port += 1;
  int datafd = socket(AF_INET, SOCK_STREAM, 0);
  char buffer[BUFF_SIZE];
  memset(buffer,0,BUFF_SIZE);

  int which_CMD = wCMD(token[1]);
  int code = errorCode(token[1],token[2],cli_addr);
  printf("code:%i\n",code);
  if(code == 221)
  {
    char* good = "221";
    write(sockfd,good,3);
    freeTokens(token,nt);
    return code;
  }
  /* good proceed */
  if(code == 200)
  {
    char cmdBuff[MAX_CMD_LEN];
    int fd; 
    switch(which_CMD)
    {
      case LIST:
        memset(cmdBuff,0,MAX_CMD_LEN);
        //snprintf(cmdBuff,MAX_CMD_LEN,"ls %s",token[2]);
        sprintf(cmdBuff,"ls %s",token[2]);
        
        printf("data conn:%i\n",conn(datafd,&data_adrr));

        FILE* ex = popen(cmdBuff,"r");
        while((n = fread(buffer,1,BUFF_SIZE,ex)) > 0){
          buffer[n]=0;
          write(datafd,buffer,BUFF_SIZE);
        }
        pclose(ex);
        break;
      case RETR:
        fd = open(token[2],O_RDONLY);
        struct stat buf;
        fstat(fd,&buf);
        read(fd,buffer,buf.st_size);
        sendFile(datafd,buffer,buf.st_size);
        close(datafd);
        close(fd);
        break;
      case STOR:
        fd = open(token[2],O_TRUNC | O_CREAT | O_WRONLY | O_APPEND);
        recvFile(sockfd,buffer,datafd);
        close(datafd);
        close(fd);
        break;

      default:
        fprintf(stderr, "invalid status code\n");
        break;

    }
    char* good = "200";
    write(sockfd,good,3);
  }
  else
  {
    //snprintf(buff,,"%i",code);
    sprintf(buff,"%i",code);
    write(sockfd,buff,3);
  }
  freeTokens(token,nt);
  return code;
  
}
/*
 * checks for error validation then returns the status code. Checks in this order:
 * 1. 500: syntax cmdBuff
 * 2. 501: syntax args
 * 3. 425: can't open data port
 * 4. 200: ok
 * 5. 452: Error writing a file is special error checking
 */
int errorCode(char* cmdBuff,char* args,struct sockaddr_in* addr)
{
  int code = wCMD(cmdBuff);
  /* known cmdBuff */
  if(code == -1)
  {
    return 500;
  }
  /* file or folder exist */
  if(code == LIST || code == RETR)
  {
    /* if args was given for get */
    if(code == RETR)
    {
      if(args[0] != 0)
      {
        int fd = open(args,O_RDONLY);
        if(fd<0)
        {
          close(fd);
          return 501;
        }
      }      
    }
  }
  /* is data port is open */
  if(addr != NULL)
  {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(conn(sockfd,addr) == -1)
    {
      return 425;
    }
    close(sockfd);
  }
  if(code == QUIT)
  {
    return 221;
  }
    return 200;
  fprintf(stderr, "erroCode: error not implemented \n");
}
void strCMD(char* cmdBuff)
{
  errorCheck((cmdBuff == NULL), "strCMD NULL\n", 0);
  if(strcmp(cmdBuff,"ls")==0)
      memcpy(cmdBuff,"LIST",4);
  else if(strcmp(cmdBuff,"get")==0)
      memcpy(cmdBuff,"RETR",4);
  else if(strcmp(cmdBuff,"put")==0)
      memcpy(cmdBuff,"STOR",4);
}
/* which cmdBuff */
int wCMD(char* cmdBuff)
{
  errorCheck((cmdBuff == NULL), "wCMD NULL\n", 0);
  if(strcmp(cmdBuff,"LIST")==0)
      return LIST;
  if(strcmp(cmdBuff,"RETR")==0)
      return RETR;
  if(strcmp(cmdBuff,"STOR")==0)
      return STOR;
  if(strcmp(cmdBuff,"QUIT")==0) 
      return QUIT;
  return -1;
}

/* custom error function wrapper throws error if true */
void errorCheck(int cond, char* errmsg, int mode)
{
  if(cond)
  {
    switch(mode) 
    {
      case -1:
        perror(errmsg);
        break;
      default:
        fprintf(stderr, "%s\n", errmsg);
        break;
    };
    exit(EXIT_FAILURE);
  }
}

/* splits the string using c */
/* for each string split it copies the string to buffer[i] */
/* needs to be freed later */
/* returns number of tokens else 0 on none and -1 on error */
int splitString(char* c,char* str,char** buffer)
{
    if(str != NULL && buffer != NULL)
    {
        if(str[0]==0) return 0;
        char *token, *string, *tofree;
        tofree = string = strdup(str);
        errorCheck(string == NULL, "splitString: strdup()\n", 0);
        int i =0;
        while((token=strsep(&string,c)) != NULL){
            buffer[i] = strdup(token);
            i++;
        }
        buffer[i] = NULL;
        free(tofree);
        return i;
    }
    else
    {
        fprintf(stderr, "split string str or buffer is null\n");
        return -1;
    }
}
void freeTokens(char** buffer,int ntoks)
{
    int i = 0;
    while( i < ntoks)
    {
        free(buffer[i++]);
    }
}
