#include "../header/stack.h"
#include <string.h>
#include <unistd.h>
int main(int argc,char** argv){

    stack* s = stack_init();
    char buffer[512];
    FILE* file = popen(argv[1],"r");
    int n =0;
    
    while((n = fread(buffer,1,512,file)) > 0){
            push_back(s,buffer,n);
    }
    writeOut(s,STDOUT_FILENO);
   
    clear(s);
    stack_deinit(s);

    return (EXIT_SUCCESS);
}
