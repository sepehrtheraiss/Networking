#include "../header/stack.h"
#include <unistd.h>
#define buff_size 512
typedef struct stack_node{
    unsigned int size;
    node* head;
    node* tail;
}stack_node;

stack_node* cast(void* n){return n;}
// returns a new stack node
void* stack_init(){
    stack_node* sPtr = malloc(sizeof(stack_node));
    sPtr->size = 0;
    sPtr->head = NULL;
    sPtr->tail = NULL;
    return sPtr;
}
void stack_deinit(void* n){
    node* ptr = cast(n)->head;
    while(ptr != NULL){
        freeNode(ptr);
        cast(n)->head = cast(n)->head->next;
        ptr = cast(n)->head; 
    }
    cast(n)->tail = NULL;
    cast(n)->size = 0;
    free(n);
}
int stack_empty(void* n){
    return (cast(n)->size == 0);
}
int stack_size(void* n){
    return cast(n)->size;
}
void* bottom(void* n){return (char*)cast(n)->tail->str;}
void* back(void* n){return (char*)cast(n)->tail->str;}
// both will return 1 on success and 0 on fail
int push_back(void* s,char* str,int l){
    //write(0,str,512);
    node* n = newNode(str,l);
    if(cast(s)->head == NULL){
        cast(s)->head = n;
        cast(s)->tail = n;
    }
    else{
        node* tmp = cast(s)->tail;
        tmp->next = n;
        n->prev = tmp;
        cast(s)->tail = n;
    }
    cast(s)->size++;
    return 1;
}
// needs to be changed such that it returns new allocated memory
char* pop_back(void* s){
    if(cast(s)->size == 0){
        printf("poping on empty stack\n");
        exit(EXIT_FAILURE);
    }
    char* str = NULL;
    if(cast(s)->tail != NULL){
        //str = malloc(sizeof(char)*cast(s)->tail->len);
        //str = cast(s)->tail->str;
        cast(s)->tail = cast(s)->tail->prev;
        if(cast(s)->tail != NULL){
            freeNode(cast(s)->tail->next);
            cast(s)->tail->next = NULL;
        }
        else{
            cast(s)->head = NULL;
        }
        //freeNode(d);
    }
    else{
        cast(s)->head = NULL;
    }
    cast(s)->size--;
    return str;
}
char* pop_bottom(void *s){
    if(cast(s)->size == 0){
    printf("poping on empty stack\n");
    exit(EXIT_FAILURE);
    }
    char* str = malloc(sizeof(char)*512);// could be optimzed using strlen 
    str = cast(s)->head->str;
    node* d = cast(s)->head;
    cast(s)->head = cast(s)->head->next;
    freeNode(d);
    if(cast(s)->head != NULL){
        cast(s)->head->prev = NULL;
    }
    if(cast(s)->tail == cast(s)->head){
        cast(s)->tail = NULL;
    }
    cast(s)->size--;
    return str;
}
void printStack(FILE* out,void* s){
    node* ptr = cast(s)->head;
    while(ptr != NULL){
        //fprintf(out,"%s",ptr->str);
        write(STDOUT_FILENO,ptr->str,buff_size);
        ptr = ptr->next;
    }
}
int writeOut(void *s,int fd){
    int n =0;
    node* ptr = cast(s)->head;
    while(ptr != NULL){
        n = write(fd,ptr->str,ptr->len);
        ptr = ptr->next;
    }
    return n;
}

void clear(void *s){
/*    while(cast(s)->size != 0){
        pop_back(s);
    }*/
 node* ptr = cast(s)->head;
    while(ptr != NULL){
        freeNode(ptr);
        cast(s)->head = cast(s)->head->next;
        ptr = cast(s)->head; 
    }
    cast(s)->tail = NULL;
    cast(s)->size = 0;

}
