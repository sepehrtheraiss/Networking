#ifndef _HEADER_STACK_
#define _HEADER_STACK_
#include "node.h"
// returns a new stack node
typedef struct stack_node stack;
void* stack_init();
void stack_deinit(void* n);
int stack_empty(void* n);
int stack_size(void* n);
void* bottom(void* n);
void* back(void* n);
// both will return 1 on success and 0 on fail
int push_back(void* s,char* str,int l);
char* pop_back(void* s);
// return new allocated string, client needs to free it.
char* pop_bottom(void* s);
// sequentially return chars from bottom to up, if ptr != NULL return 1 else 0
int bottomsUP(void *s,char* str);
int writeOut(void *s,int fd);
// clear the stack to its initial zero state
void clear(void *);
void printStack(FILE* out,void* s);
#endif
