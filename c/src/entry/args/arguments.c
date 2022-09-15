#include "arguments.h"

int init_args(int argc, char **argv, parse_handler *handler)
{
    strcpy(handler->input_file_path, "../../test/testA.cmm");
    return CMM_INIT_ARGUMENT_SUCCESS;
}
