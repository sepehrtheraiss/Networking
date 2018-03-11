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
#define BUFF_SIZE 2048
#define HTTP_PORT 80

// establish connection with the given 3 args, head,host,connection
int fetch_response(char** lines,char* host,int lines_len) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[4096];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
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
    char format[512];
    int i = 0;
    puts(lines[0]);
    while(i < lines_len)
    {
        sprintf(format,"%s\r\n",lines[i]);
        strcat(request,format);
        i++;
    }
    puts(request);
    n = write(sockfd,request,strlen(request));
    if (n < 0) perror("ERROR writing to socket");
    bzero(buffer,4096);
    n = read(sockfd,buffer,4095);
    if (n < 0) perror("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 1;
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
        memcpy(path,p,strlen(p)+1);
        if(c != NULL)
            *c = 0;
    }
    else 
        return 0;
    return 1;        
}
void test(){
    char* msg ="GET /blackGirls HTTP/1.1";
    char* string = strdup(msg);
    char* req[4];
    int i =0;
    while((req[i]=strsep(&string," ")) != NULL){
        puts(req[i]);
        i++;
    }
    free(string);
    printf("%i\n",typeReq(req[0]));
    char* host = strdup(req[1]);
    char path[strlen(host)];
    getHostPath(host,path);
    puts(host);
    puts(path);
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
    char buffer[BUFF_SIZE];
    int clisockfd;
    socklen_t clilen;
    while(stop != 1)
    {
        clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        bzero(buffer,BUFF_SIZE);

        if (clisockfd < 0) { 
         perror("ERROR on accept");
         exit(EXIT_FAILURE);
        }

        // we want to find the line with 3 spaces for request header
        read(clisockfd,buffer,BUFF_SIZE); // read clients request
        puts(buffer);
        int lines_len = cinStr('\n',buffer,BUFF_SIZE);  // number of lines 
        char* lines[lines_len];
        splitString("\n",buffer,lines); // split each line
        stripR(lines,lines_len); // stirps \r
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

            getHostPath(host,path);
            sprintf(lines[headers[0]],"%s %s %s",req,path,prot);
            // continue
            if(headers[1] == -1)
            {
                // get host from headers[0]
                // format then others
                fprintf(stderr, "no host given\n");
            }
            //printf("%s%s\n%s\n",lines[headers[0]],lines[headers[1]],lines[headers[2]]);
            fetch_response(lines,host,lines_len);
            free(host);
        }
        else
        {
            fprintf(stderr, "no request or connection given\n");
        }

    }// end main while loop



    return 0;
} 
