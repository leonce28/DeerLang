#include <string.h>
#include "common/list.h"

#ifndef nullptr
#define nullptr     NULL
#endif

static DeerLinkedList *dlist_create() 
{
    DeerLinkedList *list = malloc(sizeof(DeerLinkedList));

    if (!list) {
        return nullptr;
    }

    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;

    return list;
}

void dlist_distory(DeerLinkedList *list)
{
    DeerListCell *next = nullptr;
    DeerListCell *node = list->head;

    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    free(list);
}

void *dlist_to_array(const DeerLinkedList *list, size_t size)
{
    assert(list && size > 0);

    void *data, *buffer = nullptr, *array = nullptr;

    if (list->size == 0) {
        return nullptr;
    }

    array = malloc(list->size * size);
    buffer = array;
    assert(buffer);

    foreach (void, data, list) {
        memcpy(buffer, data, size);
        buffer += size;
    }

    return array;
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
    cell->next = nullptr;

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
        return nullptr;
    }

    const DeerListCell *curr = nullptr;
    DeerListCell *prev = nullptr, *copy = nullptr;
    DeerLinkedList *reverse = malloc(sizeof(DeerLinkedList));

    curr = list->head;

    for (; curr; copy = nullptr) {

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

DeerLinkedList *dlist_append_merge(DeerLinkedList *l1, const DeerLinkedList *l2, size_t size)
{
    void *data = nullptr;
    const DeerListCell *cell = nullptr;

    if (!l2) {
        return l1;
    }

    for (cell = l2->head; cell; cell = cell->next) {
        data = malloc(size);
        assert(data);
        memcpy(data, cell->data, size);

        l1 = dlist_push_back(l1, data);
    }

    return l1;
}


