#pragma once

#include "common/list.h"

typedef struct DeerMultiTree {
    void *data;
    DeerLinkedList *childs;
} DeerMultiTree;

extern void dtree_distory(DeerMultiTree *tree);
extern DeerMultiTree *dtree_create(void *data);
extern DeerMultiTree *dtree_append(DeerMultiTree *tree, DeerMultiTree *child);
extern DeerMultiTree *dtree_clean_clilds(DeerMultiTree *tree);