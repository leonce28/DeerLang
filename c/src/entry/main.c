#include <stdio.h>
#include "struct/parse_handler.h"
#include "arguments.h"
#include "ast/generate.h"
#include "exec/execute.h"

int main(int argc, char **argv)
{
    int ret;
    parse_handler handler;

    ret = init_args(argc, argv, handler);
    if (ret != CMM_INIT_ARGUMENT_SUCCESS) {
        printf("init arguments failed, ret: %d\n", ret);
        return ret;
    }

    ret = generate_code(handler);
    if (ret != CMM_GENERATE_CODE_SUCCESS) {
        printf("generate code failed, ret: %d\n", ret);
        return ret;
    }

    ret = execute_code(handler);
    if (ret != CMM_EXECUTE_CODE_SUCCESS) {
        printf("execute code failed, ret: %d\n", ret);
        return ret;
    }

    return 0;
}