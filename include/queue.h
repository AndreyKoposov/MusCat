#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>


typedef struct queue {
    size_t size;
    struct element* first;
    struct element* last;
} queue;

typedef struct element {
    int value;
    struct element* next;
    struct element* prev;
} element;

void queue_add(struct queue* q, int value);
int queue_get(struct queue* q);


#endif