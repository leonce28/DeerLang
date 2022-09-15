#include <stdio.h>
#include "common/global_struct.h"
#include "common/global_instance.h"
#include "kits/arguments.h"
#include "front/generate.h"
#include "back/execute.h"

int main(int argc, char **argv)
{
    int ret;
    parse_handler *handler;

    ret = get_parse_handler_instance(&handler);
    if (ret != CMM_GET_INSTANCE_SUCCESS) {
        printf("get parse handler instance failed, ret: %d\n", ret);
        return ret;
    }

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