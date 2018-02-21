#include "../include/header.h"

// this is a general test main for functions created
int main()
{
    char str0[] = "{0:512}";
    char str1[] = "{1:file_name}";
    char str2[] = "{2:file_name:i,n}";
    printf("code: %i, msg: %s\n",parse(str0,strlen(str0)),str0);
    printf("code: %i, msg: %s\n",parse(str0,strlen(str0)+10),str0);
    printf("code: %i, msg: %s\n",parse(str1,strlen(str1)),str1);
    printf("code: %i, msg: %s\n",parse(str2,strlen(str2)),str2);
    return 0;
}
