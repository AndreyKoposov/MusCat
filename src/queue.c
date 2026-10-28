#include "queue.h"


void queue_add(struct queue* q, int value) {
    struct element* new = malloc(sizeof(struct element));
    new->value = value;
    struct element* temp = q->last;
    q->last = new;
    new->next = temp;
    temp->prev = new;
    q->size++;
}

int queue_get(struct queue* q) {
    int value = q->first->value;
    struct element* temp = q->first;
    q->first = temp->prev;
    q->first->next = NULL;
    free(temp);
    q->size--;
    return value;
}
