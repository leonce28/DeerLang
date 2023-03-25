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

LinkedList *create_linked_list() {
    LinkedList *list = malloc(sizeof(LinkedList));

    if (!list) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void destroy_linked_list(LinkedList *list) {
    ListNode *next = NULL;
    ListNode *ListNode = list->head;

    while (ListNode) {
        next = ListNode->next;
        free(ListNode);
        ListNode = next;
    }

    free(list);
}

int linked_list_push_front(LinkedList *list, void *data) {
    ListNode *ListNode = malloc(sizeof(ListNode));
    if (!ListNode) {
        return 0;
    }

    ListNode->data = data;
    ListNode->next = list->head;

    if (!list->tail) {
        list->tail = ListNode;
    }

    list->head = ListNode;
    list->size++;
    return 1;
}

int linked_list_push_back(LinkedList *list, void *data) {
    ListNode *ListNode = malloc(sizeof(ListNode));
    if (!ListNode) {
        return 0;
    }

    ListNode->data = data;
    ListNode->next = NULL;

    if (!list->tail) {
        list->head = ListNode;
    } else {
        list->tail->next = ListNode;
    }

    list->tail = ListNode;
    list->size++;
    return 1;
}

int linked_list_pop_front(LinkedList *list) {
    if (!list->head) {
        return 0;
    }

    ListNode *ListNode = list->head;
    list->head = ListNode->next;
    if (!list->head) {
        list->tail = NULL;
    }

    free(ListNode);
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

    ListNode *ListNode = list->head;
    while (ListNode->next != list->tail) {
        ListNode = ListNode->next;
    }

    free(list->tail);
    ListNode->next = NULL;
    list->tail = ListNode;
    list->size--;
    return 1;
}

