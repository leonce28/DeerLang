#include <stdio.h>
#include <string.h>
#include "entry/compiler.h"

int main(int argc, char **argv)
{
    compiler_init(argc, argv);
    compiler_run();

    printf("finished.\n");
    return 0;
}