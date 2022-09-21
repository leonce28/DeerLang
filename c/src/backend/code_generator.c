#include "backend/code_generator.h"
#include "common/global_funcs.h"

int _create_code_map(const syntax_tree *ast, const symbol_table *table, code_list **codes)
{
    code_list *cl = create_code_list();

    *codes = cl;
    return CMM_SUCCESS;
}

int _merge_code_map(code_list **codes)
{
    return CMM_SUCCESS;
}

int _translate_call(code_list **codes)
{
    return CMM_SUCCESS;
}

int generate_code(const syntax_tree *ast, const symbol_table *table, parse_handler *handler, code_list **codes)
{

    if (_create_code_map(ast, table, codes)) {
        invild_call("create code map");
    }

    if (_merge_code_map(codes)) {
        invild_call("merge code map");
    }

    if (_translate_call(codes)) {
        invild_call("translate call");
    }

    return CMM_SUCCESS;
}