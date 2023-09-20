#include <stdlib.h>
#include <assert.h>
#include "backend/semantic.h"

static void analysis_var_decl(const DeerVarDecl *vd, int *var_idx, SymbolSpace *space)
{
    assert(vd && var_idx && space);
    int var_size = 0;

    // var_decl ::= type id [ '[' number ']' ] ';'
    if (vd->is_array) {
        var_size = vd->arr_size;
    }

    space->symbols = dlist_push_back(space->symbols, create_symbol(vd->id, *var_idx, var_size));

    *var_idx += var_size + 1;
}

static void analysis_func_decl(const DeerFuncDecl *fd, SymbolSpace *space)
{
    assert(fd && space);
    int var_idx = 0;
    const DeerParam *param = nullptr;
    const DeerVarDecl *var_decl = nullptr;

    // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
    if (fd->params) {

        // params :: = param { ',' param }
        foreach (DeerParam, param, fd->params) {
            // param ::= type id [ '[' ']' ]
            space->symbols = dlist_push_back(space->symbols, create_symbol(param->id, var_idx++, 0));
        }
    }

    if (fd->var_list) {

        // local_decl ::= { var_decl }
        foreach (DeerVarDecl, var_decl, fd->var_list) {
            analysis_var_decl(var_decl, &var_idx, space);
        }
    }

    // stmt_list can't define symbol, nothing to do.
}

int semantic_analysis(DeerCompilerHandle *handle)
{
    int idx = 0;
    SymbolSpace *space = nullptr;
    SymbolTable *table = nullptr;
    const DeerNode *node = nullptr;

    table->global = create_symbol_space(NAMESPACE_GLOBAL);
    table->access = create_symbol_space(NAMESPACE_ACCESS);

    // declared ::= { var_declared | func_declared }
    foreach (DeerNode, node, handle->ast->decls) {

        // global
        if (node->type == TT_VarDecl) {
            analysis_var_decl((DeerVarDecl *)node, &idx, table->global);
            continue;
        } 

        // access
        if (node->type == TT_FuncDecl && strcmp(((DeerFuncDecl *)node)->fname, NAMESPACE_ACCESS) == 0) {
            analysis_func_decl((DeerFuncDecl *)node, table->access);
            continue;
        } 

        // normal func
        space = create_symbol_space(((DeerFuncDecl *)node)->fname);
        analysis_func_decl((DeerFuncDecl *)node, space);
        table->spaces = dlist_push_back(table->spaces, space);
    }

    return CMM_SUCCESS;
}
