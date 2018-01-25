#include "../header/node.h"
#include <string.h>
/*
typedef struct Node{
    struct Node* prev;
    struct Node* next;
    char* str;
    int len;
}node;
*/
node* newNode(char* str,int l){
    node* n = malloc(sizeof(node));
    n->len = l; //strlen(str); => 518?!?! posibly because didnt use bzero
    n->str = malloc(sizeof(char)*l); 
    strcpy(n->str,str);
    n->prev = NULL;
    n->next = NULL;
    return n;
}

void freeNode(node* n){
    if(n != NULL){
        free(n->str);
        n->str = NULL;
        n->len = 0;
        n->prev = NULL;
        n->next = NULL;
        free(n);
    }
    else{
        printf("Node is NULL\n");
        exit(EXIT_FAILURE);
    }
}
