#include "../Header/node.h"
/*
typedef struct Node{
    struct Node* prev;
    struct Node* next;
    char* str;
}node;
*/
node* newNode(char* str){
    node* n = malloc(sizeof(node));
    n->str = str;
    n->prev = NULL;
    n->next = NULL;
    return n;
}

void freeNode(node* n){
    if(n != NULL){
        n->str = NULL;
        n->prev = NULL;
        n->next = NULL;
        free(n);
    }
    else{
        printf("Node is NULL\n");
        exit(EXIT_FAILURE);
    }
}
