#include "../header/stack.h"
#include <unistd.h>
#define buff_size 512
typedef struct stack_node{
    unsigned int size;
    node* head;
    node* tail;
    node** pCounter; // will be used for bottoms up, once it reaches null it gets set back to head 
}stack_node;

stack_node* cast(void* n){return n;}
// returns a new stack node
void* stack_init(){
    stack_node* sPtr = malloc(sizeof(stack_node));
    sPtr->size = 0;
    sPtr->head = NULL;
    sPtr->tail = NULL;
    sPtr->pCounter = &(sPtr->head);
    return sPtr;
}
void stack_deinit(void* n){
    node* ptr = cast(n)->head;
    while(cast(n)->head != NULL){
        cast(n)->head = cast(n)->head->next;
        freeNode(ptr);
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
int push_back(void* s,char* str){
    node* n = newNode(str);
    printf("%s", n->str);
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
char* pop_back(void* s){
    if(cast(s)->size == 0){
        printf("poping on empty stack\n");
        exit(EXIT_FAILURE);
    }
    char* str = cast(s)->tail->str;
    node* d = cast(s)->tail;
    cast(s)->tail = cast(s)->tail->prev;
    freeNode(d);
    if(cast(s)->tail != NULL){
        cast(s)->tail->next = NULL;
    }

    /*if(cast(s)->tail == cast(s)->head){
        cast(s)->head = NULL;
    }*/
    if(cast(s)->tail == NULL){
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
void writeOut(void *s,int fd){
    node* ptr = cast(s)->head;
    while(ptr != NULL){
        write(fd,ptr->str,buff_size);
        ptr = ptr->next;
    }
}
// probably best not to use
int bottomsUP(void *s,char* str){
 if(cast(s)->size == 0){
    printf("bottomsUP on empty stack\n");
    exit(EXIT_FAILURE);
    }
    if(*(cast(s)->pCounter) != NULL){
        str = (*(cast(s)->pCounter))->str;
        *(cast(s)->pCounter) = (*(cast(s)->pCounter))->next; 
        return 1;
    }
    else{
        return 0;
    }

}
void clear(void *s){
    while(cast(s)->size != 0){
        pop_back(s);
    }

}