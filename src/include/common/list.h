#pragma once

typedef struct _list list;
typedef union _list_node list_node;

extern list_node *list_tail(const list *ls);
extern list *list_append(list *ls, void *node);

#define list_first(ln)              ((ln)->ptr)
#define list_first_int(ln)          ((ln)->int_value)
#define list_first_node(type, ln)   ((type *) list_first(ln))
#define list_last(l)                list_first(list_tail(l))
#define list_last_int(l)            list_first_int(list_tail(l))
#define list_last_node(type, l)     ((type *) list_tail(l))