#include "kits/arguments.h"

extern char *optarg;
extern int getopt();

void _usage()
{
    printf("usage: compiler [option]\n");
    printf("option: \n");
    printf("  -i <file> \tinput cmm file path, default a.cmm.\n");
    printf("  -o <file> \toutput asm file path, default a.out.\n");
    printf("  -r <file> \texecute asm file, default a.out.\n");
    printf("  -h \t\tshow this help message and exit.\n");
}

int init_args(int argc, char **argv, parse_handler *handler)
{
    int op;
    while ((op = getopt(argc, (char **)argv, "ho:i:r:")) != EOF) {
        switch (op) {
            case 'o':
                strncpy(handler->output_asm_path, optarg, FILE_PATH_MAX);
                break;
            case 'i':
                strncpy(handler->input_cmm_path, optarg, FILE_PATH_MAX);
                break;
            case 'r':
                strncpy(handler->input_asm_path, optarg, FILE_PATH_MAX);
                break;
            case 'h':
            default:
                _usage();
                exit(0);
                break;
        }
    }

    printf("input_cmm_path: %s\n", handler->input_cmm_path);
    printf("output_asm_path: %s\n", handler->output_asm_path);
    printf("input_asm_path: %s\n", handler->input_asm_path);
    return CMM_INIT_ARGUMENT_SUCCESS;
}
