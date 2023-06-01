#pragma once

typedef int (*BridgeFuncFree)(void *data);
typedef int (*BridgeFuncHash)(const void *data);
typedef int (*BridgeFuncTostr)(const void *data, char *dst, int len);
typedef int (*BridgeFuncCompare)(const void *ldata, const void *rdata);