#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    size_t size;
} LinkedList;

LinkedList* create_linked_list() {
    LinkedList* list = malloc(sizeof(LinkedList));
    if (!list) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void destroy_linked_list(LinkedList* list) {
    Node* node = list->head;
    while (node) {
        Node* next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

int linked_list_push_front(LinkedList* list, int data) {
    Node* node = malloc(sizeof(Node));
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

int linked_list_push_back(LinkedList* list, int data) {
    Node* node = malloc(sizeof(Node));
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

int linked_list_pop_front(LinkedList* list) {
    if (!list->head) {
        return 0;
    }
    Node* node = list->head;
    list->head = node->next;
    if (!list->head) {
        list->tail = NULL;
    }
    free(node);
    list->size--;
    return 1;
}

int linked_list_pop_back(LinkedList* list) {
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
    Node* node = list->head;
    while (node->next != list->tail) {
        node = node->next;
    }
    free(list->tail);
    node->next = NULL;
    list->tail = node;
    list->size--;
    return 1;
}

void linked_list_print(LinkedList* list) {
    printf("size: %lu, data: [", list->size);
    Node* node = list->head;
    while (node) {
        printf("%d", node->data);
        node = node->next;
        if (node) {
            printf(", ");
        }
    }
    printf("]\n");
}

int main() {
    LinkedList* list = create_linked_list();
    if (!list) {
        return 1;
    }

    linked_list_push_front(list, 1);
    linked_list_push_front(list, 2);
    linked_list_push_front(list, 3);
    linked_list_push_back(list, 4);
    linked_list_push_back(list, 5);
    linked_list_push_back(list, 6);
    linked_list_print(list);

    linked_list_pop_front(list);
    linked_list_pop_front(list);
    linked_list_pop_back(list);
    linked_list_pop_back(list);
    linked_list_pop_front(list);

    linked_list_print(list);

    return 0;
}