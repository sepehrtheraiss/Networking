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