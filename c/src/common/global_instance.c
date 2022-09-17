#include "common/global_instance.h"
#include "common/global_funcs.h"

int get_parse_handler_instance(parse_handler **ph)
{
    *ph = (parse_handler *)malloc(sizeof(parse_handler));
    if (!*ph) {
        return CMM_FAILED;
    }

    memset(*ph, 0, sizeof(parse_handler));
    return CMM_SUCCESS;
}

int get_token_list_instance(token_list **tl)
{
    token_list *tokens = (token_list *)malloc(sizeof(token_list));
    if (!tokens) {
        return CMM_FAILED;
    }
    memset(tokens, 0, sizeof(token_list));
    (*tl) = tokens;
    return token_list_init(tokens);
}

int get_syntax_tree_instance(syntax_tree **st)
{
    syntax_tree *ast = (syntax_tree *)malloc(sizeof(syntax_tree));
    if (!ast) {
        return CMM_FAILED;
    }

    memset(ast, 0, sizeof(syntax_tree));
    (*st) = ast;
    return syntax_tree_init(ast);
}
