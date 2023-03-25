#include "list.h"

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