/*
 * C Program to Implement Queue Data Structure using Linked List
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 50

typedef struct Node
{
    char data[MAX_STR];
    struct Node *next;
} Node;

typedef struct Queue {
    struct Node *front;
    struct Node *back;
} Queue;

// FUNCTIONS DECLARATION
Queue *CreateQueue(void);
void Enq(Queue *q, char s[MAX_STR]);
void Dequeue(Queue *q);
void FreeQeueue(Queue *q);
void InsertFront(Queue *q, char s[MAX_STR]);

// Create an empty queue and initialise values
Queue *CreateQueue(void) {
    Queue *newQ = malloc(sizeof(Node*));
    newQ->back = newQ->front = NULL;
    return newQ;
}

// Insert onto the end of the queue
void InsertEnd(Queue *q, char s[MAX_STR]) {
    Node *new = malloc(sizeof(Node));
    strcat(new->data, s);

    if (q->front == NULL) {
        q->front = new;
        q->front->next = NULL;
        q->back = q->front;
    } else {
        q->back->next = new;
        q->back = q->back->next;
    }

}

// Remove from the front of the queue, does NOT return a value.
void Dequeue(Queue *q) {
    if (q == NULL) return;
    Node *next = q->front->next;
    free(q->front);
    q->front = next;
    if (q->front == NULL) q->back = NULL;
    
}

//Frees the queue memory
void FreeQeueue(Queue *q) {
    if (q == NULL) return;
    if (q->front == NULL) return;
    while (q->front != q->back) {
        Node *temp = q->front;
        q->front = q->front->next;
        free(temp);
    }
    free(q->front);
    free(q);
}

void InsertFront(Queue *q, char s[MAX_STR]) {
    if (q == NULL) return;
    Node *temp = malloc(sizeof(Node));
    strcat(temp->data, s);
    temp->next = q->front;
    q->front = temp;
}