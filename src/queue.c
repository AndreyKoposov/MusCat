#include "queue.h"


typedef struct element {
    int value;
    struct element* next;
} element;

void queue_add(struct queue* q, int value) {
    struct element* new = malloc(sizeof(struct element));
    new->value = value;
    new->next = NULL;

    if (q->size == 0) {
        q->first = new;
    }
    else if (q->size == 1) {
        q->last = new;
        q->first->next = new;
    }
    else {
        struct element* temp = q->last;
        q->last = new;
        temp->next = new;
    }
   
    q->size++;
}

int queue_get(struct queue* q) {
    struct element* temp = q->first;
    int value = temp->value;

    q->first = temp->next;

    free(temp);

    q->size--;
    return value;
}
