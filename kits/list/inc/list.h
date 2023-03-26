#pragma once

typedef struct ListNode ListNode;
typedef struct LinkedList LinkedList;
typedef void(*LinkedListPrint)(void *);

extern LinkedList *linked_list_create();
extern void linked_list_destroy(LinkedList *list);
extern int linked_list_push_front(LinkedList *list, void *data);
extern int linked_list_push_back(LinkedList *list, void *data);
extern int linked_list_pop_front(LinkedList *list);
extern int linked_list_pop_back(LinkedList *list);
extern ListNode *linked_list_node_front(const LinkedList *list);
extern ListNode *linked_list_node_back(const LinkedList *list);
extern ListNode *linked_list_node_next(const ListNode *node);
extern void *linked_list_node_data(const ListNode *node);
extern void linked_list_print(const LinkedList *list, LinkedListPrint print);
