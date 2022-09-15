#include "arguments.h"

extern char *optarg;

void _usage()
{
    printf("usage: compiler [option]\n");
    printf("option: \n");
    printf("  -i <file> \tinput cmm file path, default a.cmm.\n");
    printf("  -o <file> \toutput asm file path, default a.out.\n");
    printf("  -r <file> \tinput asm file path, then execute, default a.out.\n");
    printf("  -h \t\tshow this help message and exit.\n");
}

int init_args(int argc, char **argv, parse_handler *handler)
{
    int op;
    const char *input_cmm_path;
    const char *out_asm_path;
    const char *input_asm_path;
    while ((op = getopt(argc, (char **)argv, ":o::h::i:r:")) != EOF) {
        switch (op) {
            case 'o':
                out_asm_path = strdup(optarg);
                break;
            case 'i':
                input_cmm_path = strdup(optarg);
                break;
            case 'r':
                input_asm_path = strdup(optarg);
                break;
            case 'h':
            default:
                _usage();
                exit(0);
                break;
        }
    }

    strcpy(handler->input_file_path, "a.cmm");
    return CMM_INIT_ARGUMENT_SUCCESS;
}
