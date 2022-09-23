#include <assert.h>
#include "backend/code_generator.h"
#include "common/global_funcs.h"

int _create_unordered_code_map(const syntax_tree *ast, const symbol_table *table, unordered_code_map **ucm)
{
    assert(ast != NULL && table != NULL);
    unordered_code_map *u = create_unordered_code_map();

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
        invild_call("create code map");
    }

    if (_merge_code_map(ucm)) {
        invild_call("merge code map");
    }

    if (_translate_call(ucm)) {
        invild_call("translate call");
    }

    return CMM_SUCCESS;
}