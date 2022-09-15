#include "front/generate.h"
#include "front/lexical_analysis.h"
#include "common/global_instance.h"

int generate_code(parse_handler *handler)
{
    int ret;
    token_list *tokens;

    ret = get_token_list_instance(&tokens);
    if (ret != CMM_SUCCESS) {
        printf("get token list instance failed, ret: %d\n", ret);
        return ret;
    }

    // tokens->op->push((void *)tokens, (void *)create_token(1, "test1", 1));
    // tokens->op->push((void *)tokens, (void *)create_token(2, "test2", 2));
    // tokens->op->push((void *)tokens, (void *)create_token(3, "test3", 3));
    // tokens->op->push((void *)tokens, (void *)create_token(4, "test4", 4));
    // tokens->op->print((void *)tokens);

    ret = lexical_analysis(handler->input_cmm_path, tokens);
    if (ret != CMM_SUCCESS) {
        printf("get token list instance failed, ret: %d\n", ret);
        return ret;
    }

    return CMM_SUCCESS;
}
