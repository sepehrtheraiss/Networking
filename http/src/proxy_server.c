#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#define LISTEN 5
#define BUFF_SIZE 4096
#define HTTP_PORT 80
char HOST[256];
char FULL_RQ[1024];
int isStr(char* str,char* c)
{
    return (strstr(str,c) != NULL);
}

char* findStr(char* str,char* c)
{
    return strstr(str,c);
}
int getContentLength(char* str)
{
    char* p = findStr(str,"Content-Length:");
    if(p == NULL)
    {
        return 0;
    }
    int i =0;
    for(;p[i] != '\n';i++)
    {
        i++;
    }
    char buff[BUFF_SIZE];
    memcpy(buff,p,i);
    char* r = strchr(buff,'\n');
    if(r != NULL)
    {
        *r = 0;
    }
    r = strchr(buff,'\r');
    if(r != NULL)
    {
        *r = 0;
    }
    char *string, *tofree;
    tofree = string = strdup(buff);
    assert(string != NULL);
    char* token[3];
    i = 0;
    while ((token[i++] = strsep(&string, " ")) != NULL);
    if(token[1] != NULL)
    {
        int length = atoi(token[1]);
        free(tofree);
        return length;
    }
    return 0;
}
// returns 0 on connection close and 1 on keep it alive
// returns -1 on error
int getConnection(char* str)
{
    char* p = findStr(str,"Connection:");
    if(p == NULL)
    {
        return -1;
    }
    int i =0;
    for(;p[i] != '\n';i++)
    {
        i++;
    }
    char buff[BUFF_SIZE];
    memcpy(buff,p,i);
    char* r = strchr(buff,'\n');
    if(r != NULL)
    {
        *r = 0;
    }
    r = strchr(buff,'\r');
    if(r != NULL)
    {
        *r = 0;
    }
    char *string, *tofree;
    tofree = string = strdup(buff);
    assert(string != NULL);
    char* token[3];
    i = 0;
    while ((token[i++] = strsep(&string, " ")) != NULL);
    if(token[1] != NULL)
    {
        if(strcmp(token[1],"keep-alive")==0 || strcmp(token[1],"Keep-Alive")==0)
        {
            free(tofree);
            return 1;
        }
        else if(strcmp(token[1],"close")==0 || strcmp(token[1],"Close")==0)
        {
            free(tofree);
            return 0;
        }
    }
    return 0;
}
int getHeaderSize(char* str)
{
    int i = 0;
    int stop = 0;
    while(stop != 1)
    {
        if(str[i] == '\r' && str[i+1] == '\n' && str[i+2] == '\r' && str[i+3] == '\n')
        {
            stop = 1;
        }
        if(str[i] == 0)
        {
            stop = 1;
        }
        i++;
    }
    return i;

}
// establish connection with the given 3 args, head,host,connection
// if connection is to close returns 0 else keep it alive returns 1
int fetch_response(int sockfd,char** lines,char* host,int lines_len,int clisockfd) {
    int portno, n,bytes_read;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFF_SIZE+1];
    int keep_connection = 0;

    strcpy(host,HOST);
    server = gethostbyname(host);
    puts("proxy_server:");
    printf("HOST: %s\n",HOST);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 0;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(HTTP_PORT);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) perror("ERROR connecting");
    if(lines_len > BUFF_SIZE)
    {
        fprintf(stderr, "request size bigger than 2Kb\n");
        return -1;
    }
    char request[BUFF_SIZE];
    char format[BUFF_SIZE];
    bzero(request,BUFF_SIZE);
    int i = 0;
    // sprintf(format,"%s\r\n","GET / HTTP/1.0");
    // strcat(request,format);
    // sprintf(format,"%s\r\n","Host: www.example.com");
    // strcat(request,format);
    while(i < lines_len-1)
    {
        // mac only problem
        if(i == 1 && lines[1] == NULL){
            sprintf(format,"Host: %s\r\n",host);
            strcat(request,format);
        }
        else
        {
            sprintf(format,"%s\r\n",lines[i]);
            strcat(request,format);
        }
        /*
        // for testing
        else if(isStr(lines[i],"Accept-Encoding:") != 1)
        {
            sprintf(format,"%s\r\n",lines[i]);
            strcat(request,format);
        }
        */
        i++;
    }
    sprintf(format,"%s\r\n\r\n",lines[i]);
    strcat(request,format);
    bzero(buffer,BUFF_SIZE+1);
    puts(request); // formated output

    /* initiate the first request which returns the header and some body  */
    bytes_read = write(sockfd,request,strlen(request));
    if (bytes_read < 0) perror("ERROR writing to socket");
    bytes_read = read(sockfd,buffer,BUFF_SIZE);
    if (bytes_read < 0) perror("ERROR reading from socket");
    buffer[bytes_read]=0;
    write(clisockfd,buffer,bytes_read);
    printf("Servers response:\n%s",buffer);
    keep_connection = getConnection(buffer);
    while((bytes_read = read(sockfd,buffer,BUFF_SIZE))!=0)
    {
        if (bytes_read < 0) perror("ERROR reading from socket");
        buffer[bytes_read]=0;
        write(clisockfd,buffer,bytes_read);
       // printf("%s",buffer);
    }
    /* some fucking headers dont have content-length wtf! 
    int headerSize = getHeaderSize(buffer);
    bytes_read = bytes_read - headerSize;
    int content_size = getContentLength(buffer);
    if(content_size == 0 && headerSize != 0)
    {
        return 0;
    }
    int left_over;
    if(content_size > bytes_read)
    {
        left_over = content_size - bytes_read;
    }
    else
    {
        left_over = bytes_read - content_size;
    }
    char body[left_over+1];
    n = left_over+10;
    // let n be the total read
    while((bytes_read = read(sockfd,body,n))!=0)
    {
        if (bytes_read < 0) perror("ERROR reading from socket");
        body[bytes_read]=0;
        write(clisockfd,body,bytes_read);
        n -= bytes_read;
        
        printf("%s",body);
    }

   // printf("\nContent_Length: %i\n",content_size);
    printf("\nmissing bytes: %i\n",n);
        */
   // printf("\nfucking here\n");
    return keep_connection;
}
// 0 for invalid, 1 for HEAD and 2 for GET
int typeReq(char* str)
{
    if(strcmp(str,"HAED")==0)
    {
        return 1;
    }
    else if(strcmp(str,"GET")==0)
    {
        return 2;
    }
    else
        return 0;
}
// 0 on invalid
// 1 on success
int getHostPath(char* host,char* path){
    if(strstr(host,"https://") != NULL) {
        return 0;
    }
    char* p;
    char* c;
    if(strstr(host,"http://") != NULL)
    {
        if(host+7 != NULL)
        {
            memcpy(host,host+7,strlen(host)-6); // read 1 extra for \0
            c = strchr(host,'/');
        }
        else
            return 0;
    }
    p = strstr(host,"/");
    if(p != NULL)// && p[strlen(p)] == 0)
    {
        //printf("path:%s\n",p);
        memcpy(path,p,strlen(p)+1);
        if(c != NULL)
            *c = 0;
    }
    else 
        return 0;
    return 1;        
}
// returns the number of times character c has occured in string
int cinStr(char c,char* str,int len)
{
    int i =0;
    int n =0;
    while(i<len && str[i] != 0)
    {
        if(str[i] == c)
        {
            n++;
        }
        i++;
    }
    return n;
}
// splits the string using c 
// returns 1 on success 0 on false
int splitString(char* c,char* str,char** buffer)
{
    //puts(str);
    if(str != NULL && buffer != NULL)
    {
        char* string = strdup(str);
        int i =0;
        while((buffer[i]=strsep(&string,c)) != NULL){
            i++;
        }
        free(string);
        return 1;
    }
    else
    {
        fprintf(stderr, "split string str or buffer is null\n");
        return 0;
    }
}
// takes in a lines and tries to find the first occurance of HEAD or GET 
// finds first occurace of Host: after HAED or GET
// find the first occurance of Connection after Host: then overwrites lines from first header request to connection
// if finding was success index of req host and connection are stored
// else -1 is returned
void wrapReq(char** lines,int lines_len,int* indices)
{
    int line_index = 0;
    int s_rq = -1;
    int h_rq = -1;
    int e_rq = -1;
    char path[BUFF_SIZE];
    int arr_len = 0;
    while(line_index < lines_len)
    {
        arr_len = cinStr(' ',lines[line_index],strlen(lines[line_index])); // number of spaces in one line
        if(arr_len == 2 && s_rq == -1) // i.e. GET http://www.webscantest.com/ HTTP/1.1
        {
            char* str_arr[arr_len];
            splitString(" ",lines[line_index],str_arr); // split line by space
            if(typeReq(str_arr[0]) != 0) // valid request HEAD and GET
            {
                s_rq = line_index;
                if(indices!=NULL)
                {
                    indices[0] = s_rq; // beggining of req
                }
                else
                {
                    fprintf(stderr, "indicies null\n");
                    return;
                }
            }
        }
        else if(arr_len == 1 && e_rq == -1) // i.e. Connection: keep-alive
        {
            char* str_arr[arr_len];
            splitString(" ",lines[line_index],str_arr); // split line by space
            if(strcmp(str_arr[0],"Host:")==0)
            {
                if(str_arr[1] != NULL || strcmp(str_arr[1]," ") != 0)
                {
                    h_rq = line_index;
                    if(indices+1!=NULL)
                    {
                        indices[1] = h_rq; // host req
                    }
                    else
                    {
                        fprintf(stderr, "indicies null\n");
                        return;
                    }
                }//end outter if
            }
            else if(strcmp(str_arr[0],"Connection:")==0)
            {
                if(str_arr[1] != NULL || strcmp(str_arr[1]," ") != 0)
                {
                    e_rq = line_index;
                    if(indices+2!=NULL)
                    {
                        indices[2] = e_rq; // beggining of req
                    }
                    else
                    {
                        fprintf(stderr, "indicies null\n");
                        return;
                    }
                    line_index = lines_len;
                }//end otter if
            }
        }
        line_index++;
    }// end while loop
}
void stripR(char** lines,int len)
{
    int i=0;
    char* p;
    while(i<len)
    {
        p = strchr(lines[i],'\r');
        if(p != NULL)
        {
            *p = 0;
        }
        i++;
    }

}
int exectute(int s_sockfd,int clisockfd)
{
    char buffer[BUFF_SIZE];
    bzero(buffer,BUFF_SIZE);
    // we want to find the line with 3 spaces for request header
    read(clisockfd,buffer,BUFF_SIZE); // read clients request
    puts("clients request:");
    puts(buffer); // raw input
    int lines_len = cinStr('\n',buffer,BUFF_SIZE);  // number of lines 
    char* lines[lines_len];
    splitString("\n",buffer,lines); // split each line
    stripR(lines,lines_len); // stirps \r
    lines_len--; // last index is \0 so its useless
    /* 
     *  [0]: GET or HEAD 
     *  [1]: Host:
     *  [2]: Connection:
     */
    int headers[]={-1,-1,-1};
    wrapReq(lines,lines_len,headers);
    //printf("%i %i %i\n",headers[0],headers[1],headers[2]);
    //printf("%s\n%s\n%s\n",lines[headers[0]],lines[headers[1]],lines[headers[2]]);
    if(headers[0] != -1 && headers[2] != -1)
    {
        /* Reformating GET http://www.webscantest.com/ HTTP/1.1 to GET / HTTP/1.1*/
        int arr_len = cinStr(' ',lines[headers[0]],strlen(lines[headers[0]])); // number of spaces in one line
        char* str_arr[arr_len];
        splitString(" ",lines[headers[0]],str_arr); // split line by space
        char* req = strdup(str_arr[0]);
        char* host = strdup(str_arr[1]);
        char* prot = strdup(str_arr[2]);
        char path[BUFF_SIZE];
        //bzero(path,BUFF_SIZE);
        getHostPath(host,path);
        strcpy(HOST,host);
        sprintf(lines[headers[0]],"%s %s %s",req,path,prot);
        // continue
        if(headers[1] == -1)
        {
            // get host from headers[0]
            // format then others
            fprintf(stderr, "no host given\nrefresh page\n");
            exit(1);
        }
        //printf("%s%s\n%s\n",lines[headers[0]],lines[headers[1]],lines[headers[2]]);
        // start from where first actual header
        int keep = fetch_response(s_sockfd,lines+headers[0],host,lines_len,clisockfd);
        free(host);
        free(req);
        free(prot);
        printf("\nkeeping connection:%i\n",keep);
        return keep;

    }
    else
    {
        fprintf(stderr, "no request or connection given\n");
    }
    exit(1);
}
int main(int argc,char** argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "usage: proxy_server portnumber\n");
        exit(EXIT_FAILURE);
    }

    const int myport = atoi(argv[1]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
    }   
    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port = htons(myport);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
    }
    listen(sockfd,LISTEN);
    int stop = 0;
    
    int clisockfd;
    socklen_t clilen;
    while(stop != 1)
    {
            //printf("***32123123*entering*****\n");

        clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
           // printf("****entering*****\n");

        if (clisockfd < 0) { 
         perror("ERROR on accept");
         exit(EXIT_FAILURE);
        }
        if(fork()==0)
        {
            int s_sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (s_sockfd < 0) perror("ERROR opening socket");
            while(exectute(s_sockfd,clisockfd) > 0); // keep it alive
            //exectute(s_sockfd,clisockfd);
            //printf("****returned*****\n");
            if(close(s_sockfd) != 0){
                perror("ERROR close clisockfd");
                exit(EXIT_FAILURE);
            }
            
            if(close(clisockfd) != 0){
                perror("ERROR close clisockfd");
                exit(EXIT_FAILURE);
            }
            exit(1);
        }//end fork

    }// end main while loop

    close(sockfd);

    return 0;
} 
