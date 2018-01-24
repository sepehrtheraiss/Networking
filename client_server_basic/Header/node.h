#ifndef _HEADER_NODE_
#define _HEADER_NODE_
#include<stdio.h>
#include<stdlib.h>
typedef struct Node{
    struct Node* prev;
    struct Node* next;
    char* str;
}node;

node* newNode(char* str);
void freeNode(node* n);
#endif
