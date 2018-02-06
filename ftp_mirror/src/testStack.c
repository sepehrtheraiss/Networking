#include "../header/stack.h"
#include <string.h>
#include <unistd.h>
int main(int argc,char** argv){// going to pass in cat a big file
// need to make sure stack adds and frees correctly
    stack* stack = stack_init();
    char buffer[512];
    FILE* file = NULL; 
    int n =0;
    int i;
    
    for(int j =0;j<1;j++){
        file = popen(argv[1],"r");
        printf("stack size:%i\n",stack_size(stack));
        i=0;
        while((n = fread(buffer,1,512,file)) > 0){
            buffer[n]=0;
            push_back(stack,buffer,n+1);
        }
        printf("\nstack size after adding:%i\n",stack_size(stack));
        //writeOut(s,STDOUT_FILENO);
        clear(stack);
        printf("stack size after clearing:%i\n",stack_size(stack));
    }
    stack_deinit(stack);
    pclose(file);
    return (EXIT_SUCCESS);
}
