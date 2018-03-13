#include "../include/header.h"

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
        char* token;
        int i =0;
        while((token=strsep(&string,c)) != NULL){
        	buffer[i] = malloc(sizeof(char)*(strlen(token)));
        	strcpy(buffer[i],token);
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
void chrRemove(char** lines,int len,char c)
{
    int i=0;
    char* p;
    while(i<len)
    {
        p = strchr(lines[i],c);
        if(p != NULL)
        {
            *p = 0;
        }
        i++;
    }

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
        //printf("path length:%lu\n",strlen(p));
        memcpy(path,p,strlen(p)+1);
        path[strlen(p)+1]=0;
        if(c != NULL)
            *c = 0;
    }
    else 
        return 0;
    return 1;        
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
    //char path[BUFF_SIZE];
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

int exectute(int s_sockfd,int clisockfd,struct sockaddr_in cli_addr,struct sockaddr_in serv_addr)
{
    char buffer[BUFF_SIZE+1];
    bzero(buffer,BUFF_SIZE+1);
    // we want to find the line with 3 spaces for request header
    int n = read(clisockfd,buffer,BUFF_SIZE); // read clients request
    puts("clients request:");
    buffer[n]=0;
    puts(buffer); // raw input
    
    if(isFobidden(buffer))
    {
        fprintf(stderr, "WebSite is forbidden\n");
        Error405(clisockfd);
        exit(1);
    }
    
    int lines_len = cinStr('\n',buffer,BUFF_SIZE);  // number of lines 
    char* lines[lines_len+1];
    splitString("\n",buffer,lines); // split each line
    printf("TESTING\n");
    //chrRemove(lines,lines_len,'\n');
    chrRemove(lines,lines_len,'\r');
    for(int j =0;j<lines_len;j++)
    {
        printf("%s\n",lines[j]);
    }
        printf("here\n");
  
    exit(1);
}
int isFobidden(char* str)
{
    int i =0;
    char* buffer[4];
    while(f_sites[i] != NULL)
    {
        if(splitString(".",f_sites[i],buffer) == 0) exit(1);
        //printf("buff 2: %s\n",buffer[1]);
        if(strstr(str,buffer[1])!=NULL)
        {
            printf("isFobidden: %s\n",str );
            return 1;
        }
        i++;
    }
    return 0;
}