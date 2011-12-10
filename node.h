#include <stdio.h>
#include <stdlib.h>

/*  This node is a node in a linked list that
 *  stores all the filenames that we are going 
 *  to compare
 */
struct Node {
    struct Node* next, *prev;
    char* filename;
	int size;
};
/*  Each Node2 holds a pointer to a list of Node[s].
 *  This list will be a list of files that are all the
 *  same size.
 */
/*
struct Node2 {
    struct Node2 next;
    struct Node head;
    int fileSize;
};*/

extern struct Node* head;
extern struct Node* tail;

/* */
void addToHead(struct Node* newNode);
/* */
void addToTail(struct Node* newNode);
/*  This function adds a node after 'before' */
void addNode(struct Node* before, struct Node* newNode);
/*  This function removes (and frees) the node after 'before' */
void remNode(struct Node* before);
