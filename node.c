#include <stdio.h>
#include <stdlib.h>
#include "node.h"

struct Node* head = NULL;
struct Node* tail = NULL;

void addToHead(struct Node* newNode)
{
    newNode->next = head;
    head = newNode;
    if(head->next == NULL)
        tail = head;
}
void addToTail(struct Node* newNode)
{
    if(tail == NULL) {
        tail = newNode;
        head = newNode;
        tail->next = NULL;
    } else {
        tail->next = newNode;
        tail = tail->next;
        tail->next = NULL;
    }
//    printf("end of tail");
}
/*  This function adds a node after 'before' */
void addNode(struct Node* before, struct Node* newNode)
{
    newNode->next = before->next;
    before->next = newNode;
}
/*  This function removes (and frees) the node after 'before' */
/*  It also takes care of freeing the filename */
void remNode(struct Node* before)
{
    struct Node* tmp;
    if(before == NULL) {
        tmp = head;
        head = head->next;
    } else {
        tmp = before->next;
        before->next = before->next->next;
    }
    free(tmp->filename);
    free(tmp);
}
