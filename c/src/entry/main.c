#include <stdio.h>
#include "common/global_funcs.h"
#include "kits/file_operator.h"

int main(int argc, char **argv)
{
    parse_handler handler;

    arguments_init(argc, argv, &handler);

    generate_asm(handler.cmm_file, handler.asm_file);

    execute_code(handler.asm_file);

    printf("finished.\n");
    return 0;
}