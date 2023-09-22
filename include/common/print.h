#ifndef __DEER_COMMON_PRINT_HEADER__
#define __DEER_COMMON_PRINT_HEADER__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/struct.h"

void stp_print_space(int deep);
void stp_var_decl(const DeerVarDecl *vd, int deep);
void stp_print_param(const DeerParam *p, int deep);
void stp_raw_stmt(const DeerNode *node, int deep);
void stp_func_decl(const DeerFuncDecl *fd, int deep);

void syntax_tree_print(DeerDeclList *ast);
void symbol_space_print(const SymbolSpace *space);
void symbol_table_print(const SymbolTable *table);

void token_print(const DeerToken *token);
void token_list_print(DeerLinkedList *list);

void code_maps_print(DeerLinkedList *maps);

#endif // __DEER_COMMON_PRINT_HEADER__