#include "../include/queue.h"

queue* q_init()
{
    queue* q = malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;

}
void q_deinit(queue* q)
{
    q_clear(q);// its either already set to zero or it will 
    if(q->head == q->tail)
    {
        free(q->head);
        q->head = NULL;
        q->tail = NULL;
    }
    else
    {
        free(q->head);
        free(q->tail);
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
void q_insert(queue* q,char* str)
{
    q->size++;
    if(q->head == NULL)
    {
        q->head = malloc(sizeof(node));
        q->head->len = strlen(str);
        q->head->str = malloc(sizeof(char)*q->head->len);
        strcpy(q->head->str,str);
        q->tail = q->head;
        q->head->next = NULL;
    }
    else
    {
        q->tail->next = malloc(sizeof(node));
        q->tail = q->tail->next;
        q->tail->len = strlen(str);
        q->tail->str = malloc(sizeof(char)*q->tail->len);
        strcpy(q->tail->str,str);
        q->tail->next = NULL;
    }
}
// returns a pointer to the string 
char* q_front(queue* q)
{
    return q->head->str;
}
// removes from front, then appends the head
void q_remove(queue* q)
{
    if(q->head != NULL)
    {
        node* ptr = q->head;
        q->head = q->head->next;
        free(ptr);
        q->size--;
    }

}



