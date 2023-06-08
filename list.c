// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>

// #include "common/list.h"

// BridgeList *blist_create() 
// {
//     BridgeList *list = malloc(sizeof(BridgeList));

//     if (!list) {
//         return NULL;
//     }

//     list->head = NULL;
//     list->tail = NULL;
//     list->size = 0;
//     return list;
// }

// void blist_destroy(BridgeList *list) {
//     BridgeListNode *next = NULL;
//     BridgeListNode *node = list->head;

//     while (node) {
//         next = node->next;
//         if (node->value) {
//             free(node->value);
//         }
//         free(node);
//         node = next;
//     }

//     free(list);
// }

// void blist_push_front(BridgeList *list, BridgeNode *node) 
// {
//     assert(list && node);

//     BridgeListNode *lnode = malloc(sizeof(BridgeListNode));

//     assert(lnode);

//     lnode->value = node;
//     lnode->next = list->head;

//     if (!list->tail) {
//         list->tail = lnode;
//     }

//     list->head = lnode;
//     list->size++;
// }

// void blist_push_back(BridgeList *list, BridgeNode *node)
// {
//     assert(list && node);

//     BridgeListNode *lnode = malloc(sizeof(BridgeListNode));

//     assert(lnode);

//     lnode->value = node;
//     lnode->next = NULL;

//     if (list->tail) {
//         list->tail->next = lnode;
//     } else {
//         list->head = lnode;
//     }

//     list->tail = lnode;
//     list->size++;
// }

// void blist_pop_front(BridgeList *list) 
// {
//     assert(list);

//     if (!list->head) {
//         return;
//     }

//     BridgeListNode *lnode = list->head;
//     list->head = lnode->next;

//     if (!list->head) {
//         list->tail = NULL;
//     }

//     if (lnode->value) {
//         free(lnode->value);
//     }
//     free(lnode);
//     list->size--;
// }

// void blist_pop_back(BridgeList *list) 
// {
//     assert(list);

//     if (!list->tail) {
//         return;
//     }

//     if (list->head == list->tail) {
//         free(list->head);
//         list->head = NULL;
//         list->tail = NULL;
//         list->size = 0;
//         return;
//     }

//     BridgeListNode *lnode = list->head;
//     while (lnode->next != list->tail) {
//         lnode = lnode->next;
//     }

//     if (list->tail->value) {
//         free(list->tail->value);
//     }
//     free(list->tail);

//     lnode->next = NULL;
//     list->tail = lnode;
//     list->size--;
// }

// void blist_udf_free(BridgeList *list, BridgeFuncFree free)
// {
//     assert(list && free);

//     list->free = free;
// }

// void blist_udf_tostr(BridgeList *list, BridgeFuncTostr tostr)
// {
//     assert(list && tostr);

//     list->tostr = tostr;
// }

// void blist_udf_compare(BridgeList *list, BridgeFuncCompare compare)
// {
//     assert(list && compare);

//     list->compare = compare;
// }

// void blist_print(const BridgeList *list)
// {
//     blist_print_len(list, 512);
// }

// void blist_print_len(const BridgeList *list, int len)
// {
//     assert(list);

//     const BridgeListNode *node = NULL;
//     char *dst = (char *)malloc(len + 1);
//     dst[0] = '\0';

//     for (node = list->head; node; node = node->next) {
//         if (list->tostr) {
//             list->tostr(bnode_to_object(node->value), dst, len);
//         } else {
//             bnode_tostr(node->value, dst, len);
//         }

//         printf("%s ", dst);
//     }

//     free(dst);
// }
