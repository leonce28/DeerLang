#include <stdio.h>
#include "list.h"

typedef struct Integer {
    int val;
} Integer;

void integer_print(Integer *i) {
    printf("%d ", i->val);
}

int main() {
    LinkedList* list = create_linked_list();

    if (!list) {
        return 1;
    }
    Integer i1 = { 1 };
    Integer i2 = { 2 };
    Integer i3 = { 3 };
    Integer i4 = { 4 };
    Integer i5 = { 5 };
    Integer i6 = { 6 };

    linked_list_push_front(list, &i1);
    linked_list_push_front(list,&i2);
    linked_list_push_front(list, &i3);
    linked_list_push_back(list, &i4);
    linked_list_push_back(list, &i5);
    linked_list_push_back(list, &i6);

    linked_list_print(list, (LinkedListPrint)integer_print);

    linked_list_pop_front(list);
    linked_list_pop_front(list);
    linked_list_pop_back(list);
    linked_list_pop_back(list);
    linked_list_pop_front(list);
    printf("\n");

    linked_list_print(list, (LinkedListPrint)integer_print);
    printf("\n");


    destroy_linked_list(list);

    return 0;
}