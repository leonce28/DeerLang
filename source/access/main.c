#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "common/struct.h"
#include "common/funcs.h"
#include "common/print.h"

#include "frontend/lexical.h"
#include "frontend/syntax.h"

#include "backend/semantic.h"
#include "backend/generator.h"
#include "backend/execute.h"

static DeerCompilerHandle *handle;

////////////////////////////////////////////////////////////////////////////////
// extern
////////////////////////////////////////////////////////////////////////////////
extern char *optarg;

////////////////////////////////////////////////////////////////////////////////
// function
////////////////////////////////////////////////////////////////////////////////
static void compiler_code(DeerCompilerHandle *handle)
{
    if (lexical_analysis(handle)) {
        invalid_call("DeerLexical analysis");
    }
    token_list_print(handle->tokens);

    if (syntax_analysis(handle)) {
        invalid_call("syntax analysis");
    }
    syntax_tree_print(handle->ast);

    if (semantic_analysis(handle)) {
        invalid_call("semantic analysis");
    }
    symbol_table_print(handle->table);

    if (generate_code(handle)) {
        invalid_call("generate code");
    }
    code_list_print(handle->codes);
}

static void compiler_exec(DeerCompilerHandle *handle)
{
    assert(handle && handle->codes && handle->vm);

    int size = handle->codes->size;
    VirtualMachine *vm = handle->vm;
    Code **cs = (Code **)dlist_to_array(handle->codes, sizeof(Code));

    vm->ss = dvector_create(sizeof(Segment));

    for (vm->ip = 0; vm->ip < size; ++vm->ip) {
        execute_code(cs[vm->ip], vm);
    }
}

void compiler_init(const int argc, char **argv)
{
    assert(argv);

    int option;

    NEW(handle, DeerCompilerHandle);
    NEW(handle->lex, DeerLexical);

    while ((option = getopt(argc, (char **)argv, "ho:i:r:")) != EOF) {
        switch (option) {
            case 'r':
                strncpy(handle->input, optarg, FILE_PATH_MAX);
                break;
            case 'h':
            case 'o':
            case 'i':
            default:
                printf("usage: deer -[option]\n");
                printf("option: \n");
                // printf("  -i <file> \tinput cmm file path, default a.deer.\n");
                // printf("  -o <file> \toutput asm file path, default a.out.\n");
                printf("  -r <file> \trun a deer file, default ../test/case/case1.deer.\n");
                printf("  -h \t\tshow this help message and exit.\n");
                exit(0);
                break;
        }
    }
}

int compiler_read(DeerCompilerHandle *handle)
{
    assert(handle);

    FILE *fp = nullptr;
    size_t length = 0, count;

    fp = fopen(handle->input, "r");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    handle->content = (char *)malloc(length);
    assert(handle->content);

    count = fread(handle->content, length, 1, fp);
    assert(count == 1);

    fclose(fp);
    return CMM_SUCCESS;
}

int main(int argc, char **argv)
{
    compiler_init(argc, argv);

    if (strlen(handle->input) == 0) {
        strcpy(handle->input, DEFAULT_INPUT_DEER);
    }
    assert(strlen(handle->input) > 0);

    compiler_read(handle);
    assert(handle->content);

    compiler_code(handle);
    assert(handle->codes);

    compiler_exec(handle);

    printf("finished.\n");
    return 0;
}