#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct DeerListCell {
    void *data;
    struct DeerListCell *next;
} DeerListCell;

typedef struct DeerLinkedList {
    int size;
    DeerListCell *head;
    DeerListCell *tail;
} DeerLinkedList;

#define dcell_next(cell)            ((cell) ? ((cell)->next) : (NULL))

#define dcell_data(cell)            ((cell) ? ((cell)->data) : (NULL))
#define dcell_data2(_t_, _c_)       ((_t_ *) ((_c_)->data))

#define dlist_head(list)            ((list) ? ((list)->head) : (NULL))

#define foreach(__type__, __cell__, __list__) \
    for (DeerListCell *deer_##__cell__ = ((__list__)->head); \
        (deer_##__cell__ && \
        (__cell__ = (__type__ *)((deer_##__cell__)->data)) != NULL); \
        (deer_##__cell__ = (deer_##__cell__)->next))

extern DeerLinkedList *dlist_push_back(DeerLinkedList *list, void *data);
extern void dlist_distory(DeerLinkedList *list);
