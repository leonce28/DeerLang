#include <string.h>
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

DeerLinkedList *dlist_reverse_copy(const DeerLinkedList *list, size_t size)
{
    if (!list || list->size == 0 || size <= 0) {
        return NULL;
    }

    const DeerListCell *curr = NULL;
    DeerListCell *prev = NULL, *copy = NULL;
    DeerLinkedList *reverse = malloc(sizeof(DeerLinkedList));

    curr = list->head;

    for (; curr; copy = NULL) {

        copy = (DeerListCell *)malloc(sizeof(DeerListCell));
        assert(copy);
        copy->data = malloc(size);
        assert(copy->data);
        memcpy(copy->data, curr->data, size);

        copy->next = prev;

        curr = curr->next;

        prev = copy;
    }

    reverse->head = prev;
    reverse->tail = list->head;
    reverse->size = list->size;

    return reverse;
}

