#include <stdio.h>
#include "common/global_funcs.h"
#include "backend/execute_asm.h"
#include "kits/generate.h"

int main(int argc, char **argv)
{
    parse_handler handler;

    if (arguments_init(argc, argv, &handler)) {
        invild_call("init arguments");
    }

    if (generate_asm(&handler)) {
        invild_call("generate code");
    }

    if (execute_code(&handler)) {
        invild_call("execute code");
    }

    printf("finished.\n");
    return 0;
}