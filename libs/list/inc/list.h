#pragma once

typedef struct ListNode ListNode;
typedef struct LinkedList LinkedList;
typedef void(*LinkedListPrint)(void *);

extern LinkedList *create_linked_list();
extern void destroy_linked_list(LinkedList *list);
extern int linked_list_push_front(LinkedList *list, void *data);
extern int linked_list_push_back(LinkedList *list, void *data);
extern int linked_list_pop_front(LinkedList *list);
extern int linked_list_pop_back(LinkedList *list);
extern void linked_list_print(const LinkedList *list, LinkedListPrint print);
