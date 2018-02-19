#ifndef _HEADER_QUEUE_
#define _HEADER_QUEUE_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct node
{
   struct node* next;
    char* str;
    int len;
}node;
typedef struct queue
{
    int size;
    node* head;
    node* tail;
}queue;

// init queue
queue* q_init();
// deinit queue calls clear
void q_deinit(queue* q);
// if stack != 0 clear the nodes
void q_clear(queue* q);
// returns number of nodes
int q_size(queue* q);
// adds to the front of the queue
void q_insert(queue* q,char* str);
// returns a pointer to the string 
char* q_front(queue* q);
// removes from front
void q_remove(queue* q);

#endif
