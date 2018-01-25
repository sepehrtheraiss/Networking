#ifndef _HEADER_NODE_
#define _HEADER_NODE_
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct Node{
    struct Node* prev;
    struct Node* next;
    char* str;
    int len;
}node;

node* newNode(char* str,int l);
void freeNode(node* n);
#endif
