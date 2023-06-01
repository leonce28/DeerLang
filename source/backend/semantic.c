#include <stdlib.h>
#include <assert.h>
#include "backend/semantic.h"


static void _analysis_local_declared(const syntax_tree *local_declared, int *var_idx, symbol_space **space)
{
    assert(local_declared != NULL && space != NULL);
    int idx, var_size = 0;
    syntax_tree *node;

    // local_decl ::= { var_decl }
    for (idx = 0; idx < local_declared->sub_idx; ++idx) {

        // var_decl ::= type id [ '[' numbswitcher ']' ] ';'
        node = local_declared->sub_list[idx];
        var_size = node->sub_list[2] != NULL ? atoi(node->sub_list[2]->data->token_str) : 0;

        (*space)->s[(*space)->s_idx++] = create_symbol(node->sub_list[1]->data->token_str, *var_idx, var_size);
        *var_idx += var_size + 1;
    }
}

static void _analysis_func_declared(const syntax_tree *func_declared, symbol_table **table)
{
    assert(func_declared != NULL && table != NULL);

    int idx, var_idx = 0;
    syntax_tree *node, *sub;
    symbol_space *space = get_symbol_space(table, func_declared->sub_list[1]->data->token_str);

    // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
    if (func_declared->sub_list[2] != NULL) {
        
        // params ::= param_list
        // param_list :: = param { ',' param }
        for (idx = 0, node = func_declared->sub_list[2]; idx < node->sub_idx; ++idx) {

            // param ::= type id [ '[' ']' ]
            sub = node->sub_list[idx];

            space->s[space->s_idx++] = create_symbol(sub->sub_list[1]->data->token_str, var_idx++, 0);
        }
    }

    // local_decl 
    _analysis_local_declared(func_declared->sub_list[3], &var_idx, &space);

    // stmt_list todo
}

int semantic_analysis(compiler_handle *handle)
{
    int ast_idx, global_idx = 0;
    const syntax_tree *declared;
    symbol_space *global_space;
    handle->table = create_symbol_table();

    for (ast_idx = 0; ast_idx < handle->ast->sub_idx; ++ast_idx) {
        
        // declared ::= var_declared | func_declared
        declared = handle->ast->sub_list[ast_idx];

        if (TOKEN_TYPE_MATCH(declared, TOKEN_FUNC_DECL)) {
            _analysis_func_declared(declared, &handle->table);
        } else {
            global_space = get_global_space(handle->table);
            _analysis_local_declared(declared, &global_idx, &global_space);
        }
    }

    return CMM_SUCCESS;
}
