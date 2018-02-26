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
    int off_bytes[2]; // stores offset and bytes read
}node;
typedef struct queue
{
    int size;
    char file_name[32];
    int off_bytes[2]; // stores offset and bytes read
    int bytes_read;
    node* head;
    node* tail;
}queue;

// init queue
queue* q_init(char* file_name,int offset,int bytes);
// deinit queue calls clear
void q_deinit(queue* q);
// if stack != 0 clear the nodes
void q_clear(queue* q);
// returns number of nodes
int q_size(queue* q);
// adds to the front of the queue
void q_insert(queue* q,char* str,int offset,int bytes);
// returns a pointer to the string 
node* q_front(queue* q);
// removes from front
void q_remove(queue* q);
// traverses the queue and looks if a node with this specifications exist
// returns 1 on true and 0 on false
int q_exist(queue* q,char* str,int offset,int bytes);
// returns total of bytes read
int q_bytesRead(queue* q);

#endif
