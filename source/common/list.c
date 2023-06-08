#include "common/list.h"

static DeerLinkedList *dlist_create() 
{
    DeerLinkedList *list = malloc(sizeof(DeerLinkedList));

    if (!list) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}

void dlist_distory(DeerLinkedList *list)
{
    DeerListCell *next = NULL;
    DeerListCell *node = list->head;

    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    free(list);
}


DeerLinkedList *dlist_push_back(DeerLinkedList *list, void *data)
{
    assert(data);

    if (!list) {
        list = dlist_create();
    }

    assert(list);

    DeerListCell *cell = malloc(sizeof(DeerListCell));

    assert(cell);

    cell->data = data;
    cell->next = NULL;

    if (list->tail) {
        list->tail->next = cell;
    } else {
        list->head = cell;
    }

    list->tail = cell;
    list->size++;

    return list;
}

