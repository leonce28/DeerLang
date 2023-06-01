#pragma once

#include "func.h"
#include "node.h"

typedef struct BridgeList BridgeList;

extern BridgeList *blist_create();
extern void blist_destroy(BridgeList *list);

extern void blist_push_front(BridgeList *list, BridgeNode *node);
extern void blist_push_back(BridgeList *list, BridgeNode *node);

extern void blist_pop_front(BridgeList *list);
extern void blist_pop_back(BridgeList *list);

extern void blist_udf_free(BridgeList *list, BridgeFuncFree free);
extern void blist_udf_tostr(BridgeList *list, BridgeFuncTostr tostr);
extern void blist_udf_compare(BridgeList *list, BridgeFuncCompare compare);

extern void blist_print(const BridgeList *list);
extern void blist_print_len(const BridgeList *list, int len);
