#ifndef _HEADER_STACK_
#define _HEADER_STACK_
#include "node.h"
// returns a new stack node
typedef struct stack_node stack;
void* stack_init();
void stack_deinit(void* n);
int stack_empty(void* n);
int stack_size(void* n);
void* back(void* n);
// both will return 1 on success and 0 on fail
int push_back(void* s,char* str);
char* pop_back(void* s);
char* pop_bottom(void* s);
void printStack(FILE* out,void* s);
#endif
