#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "common/list.h"

#define LIST_CAPACITY_DEFAULT   1024
#define LIST_NULL               ((list *) NULL)

typedef enum _node_tag {
    LIST_NODE_TAG_NUM,
    LIST_NODE_TAG_PTR
} node_tag;

union _list_node {
    int num;
    void *ptr;
};

struct _list {
    node_tag tag;
    int size;
    int capacity;
    list_node *eptr;
    list_node elements[];
};

list_node *list_tail(const list *ls)
{
	return ls ? &ls->eptr[ls->size - 1] : NULL;
}

list *list_new(node_tag tag) 
{
    list *newlist;
    int capacity = LIST_CAPACITY_DEFAULT;

    newlist = (list *) malloc(sizeof(list) + capacity * sizeof(list_node));
    newlist->tag = tag;
    newlist->size = 1;
    newlist->capacity = capacity;
    newlist->eptr = newlist->elements;

    return newlist;
}

void list_expand(list *ls, int new_capacity)
{
    ls->eptr = (list_node *) malloc(new_capacity * sizeof(list_node));

    assert(ls->eptr);
    memcpy(ls->eptr, ls->elements, ls->size * sizeof(list_node));

    free(ls->elements);

    ls->elements[0] = *ls->eptr;
    ls->capacity = new_capacity;
}

void list_new_tail(list *ls)
{
    if (ls->size >= ls->capacity) {
        list_expand(ls, ls->capacity * 2);
    }
    ls->size++;
}

list *list_append(list *ls, void *node)
{
    assert(ls && node);

    if (ls == LIST_NULL) {
        ls = list_new(LIST_NODE_TAG_PTR);
    } else {
        list_new_tail(ls);
    }

    if(node == LIST_NULL){
        return ls;
    }

    list_first(list_tail(ls)) = node;
    return ls;
}