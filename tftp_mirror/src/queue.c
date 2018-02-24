#include "../include/queue.h"

queue* q_init(char* file_name)
{
    queue* q = malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->bytes_read = 0;
    strcpy(q->file_name,file_name);
    return q;
}
void q_deinit(queue* q)
{
    if(q != NULL)
    {
        q_clear(q);
        free(q);
    }
}
void q_clear(queue* q)
{
    while(q_size(q) != 0)
    {
         q_remove(q);   
    }
}
int q_size(queue* q)
{
    return q->size;
}
// adds to the front of the queue
void q_insert(queue* q,char* str,uint32_t offset,uint32_t bytes)
{
    if(q->head == NULL)
    {
        q->head = malloc(sizeof(node));
        q->head->len = strlen(str);
        q->head->str = malloc(sizeof(char)*q->head->len+1);
        q->head->off_bytes[0] = offset;
        q->head->off_bytes[1] = bytes;
        q->bytes_read += bytes;
        strcpy(q->head->str,str);
        q->tail = q->head;
        q->head->next = NULL;
    }
    else
    {
        q->tail->next = malloc(sizeof(node));
        q->tail = q->tail->next;
        q->tail->len = strlen(str);
        q->tail->str = malloc(sizeof(char)*q->tail->len+1);
        q->tail->off_bytes[0] = offset;
        q->tail->off_bytes[1] = bytes;
        q->bytes_read += bytes;
        strcpy(q->tail->str,str);
        q->tail->next = NULL;
    }
    q->size++;
}
// returns a pointer to the string 
node* q_front(queue* q)
{
    return q->head;
}
// removes from front, then appends the head
void q_remove(queue* q)
{
    if(q->head != NULL)
    {
        node* ptr = q->head;
        q->head = q->head->next;
        free(ptr->str);
        free(ptr);
        q->size--;
    }
}

int q_exist(queue* q,char* str,uint32_t offset,uint32_t bytes)
{
    node* ptr = q->head;
    while(ptr != NULL)
    {
        if(strcmp(ptr->str,str) == 0 && ptr->off_bytes[0] == offset && ptr->off_bytes[1] == bytes)
        {
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

uint32_t q_bytesRead(queue* q)
{
    return q->bytes_read;
}
