#include <assert.h>
#include "backend/code_generator.h"
#include "common/global_funcs.h"

int _create_unordered_code_map(const syntax_tree *ast, const symbol_table *table, unordered_code_map **ucm)
{
    assert(ast != NULL && table != NULL);

    int idx;
    code_map *map = NULL;
    code_list *cl = NULL;
    const char* cur_func_name;
    const syntax_tree *node;
    unordered_code_map *u = create_unordered_code_map();

    // declared_list ::= declared { declared }
    for (idx = 0; idx < ast->sub_idx; ++idx) {
        node = ast->sub_list[idx];

        // declared ::= local_declared | func_declared
        if (node->data->token_type == TOKEN_FUNC_DECL) {
            
            // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
            cur_func_name = node->sub_list[1]->data->token_str;

            _generate_stmt_list_code(node->sub_list[4], cur_func_name, cl);

            if (strcmp("main", cur_func_name) != 0) {
                cl_push_back(cl, INS_RET, "");
            }

            set_code_map(u, cur_func_name, cl);
        }
    }

    *ucm = u;
    return CMM_SUCCESS;
}

int _merge_code_map(unordered_code_map **ucm)
{
    return CMM_SUCCESS;
}

int _translate_call(unordered_code_map **ucm)
{
    return CMM_SUCCESS;
}

int generate_code(const syntax_tree *ast, const symbol_table *table, unordered_code_map **ucm)
{  
    if (_create_unordered_code_map(ast, table, ucm)) {
        invalid_call("create code map");
    }

    if (_merge_code_map(ucm)) {
        invalid_call("merge code map");
    }

    if (_translate_call(ucm)) {
        invalid_call("translate call");
    }

    return CMM_SUCCESS;
}