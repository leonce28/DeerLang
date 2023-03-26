#include <stdio.h>
#include <string.h>
#include "access/compiler.h"

////////////////////////////////////////////////////////////////////////////////
// extern
////////////////////////////////////////////////////////////////////////////////
extern char *optarg;
extern int getopt();

////////////////////////////////////////////////////////////////////////////////
// function
////////////////////////////////////////////////////////////////////////////////
static void generate_asm(compiler_handle *handle)
{
    if (file_read_content(handle->cmm_file, &handle->file_content)) {
        invalid_call("file read content");
    }

    if (lexical_analysis(handle)) {
        invalid_call("lexical analysis");
    }

    linked_list_print(handle->tokens, (LinkedListPrint)token_print);

    if (syntax_analysis(handle)) {
        invalid_call("syntax analysis");
    }
    // syntax_tree_print(handle->ast);

    if (semantic_analysis(handle)) {
        invalid_call("semantic analysis");
    }
    // symbol_table_print(table);

    if (generate_code(handle)) {
        invalid_call("generate code");
    }
    // code_list_print(codes);

    if (file_write_content(handle->codes, handle->asm_file)) {
        invalid_call("output asm file");
    }
}

static void execute_code(compiler_handle *handle)
{
    if (file_read_content(handle->asm_file, &handle->file_content)) {
        invalid_call("file read content");
    }

    if (load_code_segment(handle->file_content, &handle->cs)) {
        invalid_call("load code list");
    }

    if (vm_execute(handle->cs)) {
        invalid_call("virtual machine execute");
    }
}

void compiler_init(const int argc, char **argv)
{
    int option;

    assert(argv != NULL);

    NEW(handle, compiler_handle);
    NEW(handle->lex, lexical);
    NEW(handle->generator, code_generator);

    while ((option = getopt(argc, (char **)argv, "ho:i:r:")) != EOF) {
        switch (option) {
            case 'o':
                strncpy(handle->asm_file, optarg, FILE_PATH_MAX);
                break;
            case 'i':
                strncpy(handle->cmm_file, optarg, FILE_PATH_MAX);
                break;
            case 'r':
                strncpy(handle->asm_file, optarg, FILE_PATH_MAX);
                break;
            case 'h':
            default:
                printf("usage: compiler [option]\n");
                printf("option: \n");
                printf("  -i <file> \tinput cmm file path, default a.cmm.\n");
                printf("  -o <file> \toutput asm file path, default a.out.\n");
                printf("  -r <file> \texecute asm file, default a.out.\n");
                printf("  -h \t\tshow this help message and exit.\n");
                exit(0);
                break;
        }
    }

    handle->generator->node = NULL;
    handle->generator->c_map = create_code_map();
    handle->generator->cl = create_code_list();
    handle->generator->codes = create_code_list();
    handle->generator->jumps = create_jump_map();
}

void compiler_run()
{
    if (!PATH_IS_EMPTY(handle->cmm_file)) {
        generate_asm(handle);
    }

    if (!PATH_IS_EMPTY(handle->asm_file)) {
        execute_code(handle);
    }
}

int main(int argc, char **argv)
{
    compiler_init(argc, argv);
    compiler_run();

    printf("finished.\n");
    return 0;
}