#pragma once

#include "func.h"
#include "node.h"

typedef struct BridgeMap BridgeMap;
typedef struct BridgePair BridgePair;

extern BridgePair *bpair_make(BridgeNode *first, BridgeNode *second);
extern void bpair_destroy(BridgePair *pair);

extern BridgeNode *bpair_first(BridgePair *pair);
extern BridgeNode *bpair_second(BridgePair *pair);

BridgeMap *bmap_create();
void bmap_destroy(BridgeMap *map);

extern void bmap_put_pair(BridgeMap *map, BridgePair *pair);
extern BridgePair *bmap_find_pair(const BridgeMap *map, const BridgeNode *first);
extern void bmap_erase_pair(BridgeMap *map, const BridgeNode *first);

extern void bmap_udf_free(BridgeMap *map, BridgeFuncFree free);
extern void bmap_udf_hash(BridgeMap *map, BridgeFuncHash hash);
extern void bmap_udf_tostr(BridgeMap *map, BridgeFuncTostr tostr);
extern void bmap_udf_compare(BridgeMap *map, BridgeFuncCompare compare);

extern void bmap_print(const BridgeMap *map);
extern void bmap_print_len(const BridgeMap *map, int len);


