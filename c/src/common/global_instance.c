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
