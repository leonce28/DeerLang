#include <stdio.h>
#include "common/global_struct.h"
#include "frontend/generate.h"
#include "backend/execute.h"
#include "kits/arguments.h"

int main(int argc, char **argv)
{
    int ret;
    parse_handler handler;

    ret = init_args(argc, argv, &handler);
    if (ret != CMM_SUCCESS) {
        printf("init arguments failed, ret: %d\n", ret);
        return ret;
    }

    ret = generate_code(&handler);
    if (ret != CMM_SUCCESS) {
        printf("generate code failed, ret: %d\n", ret);
        return ret;
    }

    ret = execute_code(&handler);
    if (ret != CMM_SUCCESS) {
        printf("execute code failed, ret: %d\n", ret);
        return ret;
    }

    printf("finished.\n");
    return 0;
}