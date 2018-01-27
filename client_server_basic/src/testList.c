#include "../header/List.h"
#include <string.h>
#include <unistd.h>
int main(int argc,char** argv){// going to pass in cat a big file
// need to make sure stack adds and frees correctly
    List l = newList();
    char buffer[512];
    FILE* file = NULL; 
    int n =0;
    int i;
    
    for(int j =0;j<1;j++){
        file = popen(argv[1],"r");
        //printf("stack size:%i\n",stack_size(stack));
        i=0;
        while((n = fread(buffer,1,512,file)) > 0){
                append(l,buffer,n);
                printf("%i, ",i++);
        }
        //printf("\nstack size after adding:%i\n",stack_size(stack));
        //writeOut(s,STDOUT_FILENO);
        clear(l);
        //printf("stack size after clearing:%i\n",stack_size(stack));
    }
    freeList(&l);
    return (EXIT_SUCCESS);
}
