#include <stdio.h>
#include "list.h"

void integer_print(int *i) {
    printf("%d ", *i);
}

int main() {
    LinkedList *list = linked_list_create();

    if (!list) {
        return 1;
    }

    int val = 0;

    linked_list_push_front(list, &val);
    linked_list_push_front(list,&val);
    linked_list_push_front(list, &val);
    linked_list_push_back(list, &val);
    linked_list_push_back(list, &val);
    linked_list_push_back(list, &val);

    linked_list_print(list, (LinkedListPrint)integer_print);

    linked_list_pop_front(list);
    linked_list_pop_front(list);
    linked_list_pop_back(list);
    linked_list_pop_back(list);
    linked_list_pop_front(list);
    printf("\n");

    linked_list_print(list, (LinkedListPrint)integer_print);
    printf("\n");


    linked_list_destroy(list);

    return 0;
}