#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct ListNode {
    void *data;
    struct ListNode *next;
};

struct LinkedList {
    ListNode *head;
    ListNode *tail;
    size_t size;
};

LinkedList *linked_list_create() {
    LinkedList *list = malloc(sizeof(LinkedList));

    if (!list) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void linked_list_destroy(LinkedList *list) {
    ListNode *next = NULL;
    ListNode *node = list->head;

    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    free(list);
}

int linked_list_push_front(LinkedList *list, void *data) {
    ListNode *node = malloc(sizeof(ListNode));
    if (!node) {
        return 0;
    }

    node->data = data;
    node->next = list->head;

    if (!list->tail) {
        list->tail = node;
    }

    list->head = node;
    list->size++;
    return 1;
}

int linked_list_push_back(LinkedList *list, void *data) {
    ListNode *node = malloc(sizeof(ListNode));
    if (!node) {
        return 0;
    }

    node->data = data;
    node->next = NULL;

    if (!list->tail) {
        list->head = node;
    } else {
        list->tail->next = node;
    }

    list->tail = node;
    list->size++;
    return 1;
}

int linked_list_pop_front(LinkedList *list) {
    if (!list->head) {
        return 0;
    }

    ListNode *node = list->head;
    list->head = node->next;
    if (!list->head) {
        list->tail = NULL;
    }

    free(node);
    list->size--;
    return 1;
}

int linked_list_pop_back(LinkedList *list) {
    if (!list->tail) {
        return 0;
    }

    if (list->head == list->tail) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
        return 1;
    }

    ListNode *node = list->head;
    while (node->next != list->tail) {
        node = node->next;
    }

    free(list->tail);
    node->next = NULL;
    list->tail = node;
    list->size--;
    return 1;
}

ListNode *linked_list_node_front(const LinkedList *list)
{
    return list->head;
}

ListNode *linked_list_node_back(const LinkedList *list)
{
    return list->tail;
}

ListNode *linked_list_node_next(const ListNode *node)
{
    return node->next;
}

void *linked_list_node_data(const ListNode *node)
{
    return node->data;
}

void linked_list_print(const LinkedList *list, LinkedListPrint print)
{
    ListNode *node = list->head;
    while (node) {
        print(node->data);
        node = node->next;
    }
}
