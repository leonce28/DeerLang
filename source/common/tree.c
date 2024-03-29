#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <string.h>
#include "common/tree.h"

DeerNode *dtree_new_node(size_t s, TreeType t)
{
    DeerNode *node; 
    assert(s >= sizeof(DeerNode));
    node = (DeerNode *) malloc(s); 
    memset(node, 0, s);
    node->type = t; 
    return node; 
}

// void dtree_distory(DeerMultiTree *tree)
// {
//     if (!tree || !tree->childs) {
//         return;
//     }

//     DeerMultiTree *child = NULL;

//     foreach (DeerMultiTree, child, tree->childs) {
//         dtree_distory(child);
//     }

//     dlist_distory(tree->childs);
//     tree->childs = NULL;
// }

// DeerMultiTree *dtree_create(void *data)
// {
//     DeerMultiTree *tree = malloc(sizeof(DeerMultiTree));
//     assert(tree);

//     tree->data = data;
//     tree->childs = NULL;
    
//     return tree;
// }

// DeerMultiTree *dtree_append(DeerMultiTree *tree, DeerMultiTree *child)
// {
//     if (!tree || !child) {
//         return tree;
//     }

//     tree->childs = dlist_push_back(tree->childs, child);

//     assert(tree->childs);

//     return tree;
// }

// DeerMultiTree *dtree_clean_clilds(DeerMultiTree *tree)
// {
//     assert(tree);

//     DeerMultiTree *child = NULL;

//     foreach (DeerMultiTree, child, tree->childs) {
//         dtree_distory(child);
//     }

//     dlist_distory(tree->childs);
//     tree->childs = NULL;

//     return tree;
// }
