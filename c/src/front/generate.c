#include "front/generate.h"
#include "front/lexical_analysis.h"
#include "common/global_instance.h"

int generate_code(parse_handler *handler)
{
    int ret;
    vector tokens;

    ret = get_token_list_instance(&tokens);
    if (ret != CMM_SUCCESS) {
        printf("get token list instance failed, ret: %d\n", ret);
        return ret;
    }

    ret = lexical_analysis(handler->input_cmm_path, tokens);
    if (ret != CMM_SUCCESS) {
        printf("get token list instance failed, ret: %d\n", ret);
        return ret;
    }

    return CMM_SUCCESS;
}
