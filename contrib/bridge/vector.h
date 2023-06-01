#pragma once

#include "func.h"
#include "node.h"

typedef struct BridgeVector BridgeVector;

extern BridgeVector *bvector_create();
extern void bvector_destroy(BridgeVector *vector);

extern void bvector_append(BridgeVector *vector, const BridgeNode *node);
extern BridgeNode *bvector_fetch(BridgeVector *vector, int index);

extern void bvector_set_integer(BridgeVector *vector, int index, long long ival);
extern void bvector_set_decimal(BridgeVector *vector, int index, long double dval);
extern void bvector_set_string(BridgeVector *vector, int index, const char *sval);
extern void bvector_set_object(BridgeVector *vector, int index, void *oval);

extern void bvector_udf_free(BridgeVector *vector, BridgeFuncFree free);
extern void bvector_udf_tostr(BridgeVector *vector, BridgeFuncTostr tostr);
extern void bvector_udf_compare(BridgeVector *vector, BridgeFuncCompare free);

extern void bvector_print(const BridgeVector *vector);
extern void bvector_print_len(const BridgeVector *vector, int len);

extern int bvector_size(BridgeVector *vector);


