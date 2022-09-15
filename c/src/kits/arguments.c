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

    exit(0);
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
                break;
        }
    }

    if (strlen(handler->input_cmm_path) == 0) {
        strcpy(handler->input_cmm_path, "a.cmm");
        printf("use cmm default compiler file: a.cmm.\n");
    }
    if (strlen(handler->output_asm_path) == 0) {
        strcpy(handler->output_asm_path, "a.out");
        printf("use asm default output file: a.out.\n");
    }
    if (strlen(handler->input_asm_path) == 0) {
        strcpy(handler->input_asm_path, "a.out");
        printf("use asm default input file: a.out.\n");
    }

    return CMM_SUCCESS;
}
