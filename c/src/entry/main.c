#include <stdio.h>
#include "common/global_funcs.h"
#include "kits/file_operator.h"

int main(int argc, char **argv)
{
    parse_handler handler;

    if (arguments_init(argc, argv, &handler)) {
        invild_call("init arguments");
    }

    if (generate_asm(handler.cmm_file, handler.asm_file)) {
        invild_call("generate code");
    }

    if (execute_code(handler.asm_file)) {
        invild_call("execute code");
    }

    printf("finished.\n");
    return 0;
}