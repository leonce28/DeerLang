#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "common/struct.h"
#include "common/funcs.h"

#include "frontend/lexical.h"
#include "frontend/syntax.h"

#include "backend/semantic.h"
#include "backend/generator.h"
#include "backend/execute.h"

static DeerCompilerHandle *handle;

////////////////////////////////////////////////////////////////////////////////
// debug
////////////////////////////////////////////////////////////////////////////////
void deer_token_list_print(DeerLinkedList *list) 
{
    const DeerToken *token = nullptr;
    foreach (DeerToken, token, list) {
        printf("%s ", token->token_str);
    }
    printf("\n");
}

void stp_print_space(int deep)
{
    while (deep > 0) {
        printf("  ");
        deep -= 1;
    }
}

void stp_var_decl(const DeerVarDecl *vd, int deep)
{
    stp_print_space(deep);
    printf("var decl: {\n");

    if (vd->vt != VT_UNDEFINE) {
        stp_print_space(deep + 1);
        printf("var type: %s\n", vd->vt == VT_INT ? "INT" : "VOID");
    }

    if (vd->id) {
        stp_print_space(deep + 1);
        printf("id: %s\n", vd->id);
    }

    if (vd->scope) {
        stp_print_space(deep + 1);
        printf("scope: %s\n", vd->scope);
    }

    stp_print_space(deep + 1);
    printf("is local: %s\n", vd->is_local ? "true" : "false");

    stp_print_space(deep + 1);
    printf("is array: %s, %d\n", vd->is_array ? "true" : "false", vd->arr_size);

    stp_print_space(deep);
    printf("}\n");
}

void stp_print_param(const DeerParam *p, int deep)
{
    stp_print_space(deep);

    if (p->vt != VT_UNDEFINE) {
        stp_print_space(deep);
        printf("var type: %s, ", p->vt == VT_INT ? "INT": "VOID");
        printf("param id: %s \n", p->id);
    }
}

void stp_raw_stmt(const DeerNode *node, int deep)
{
    stp_print_space(deep);
    switch (node->type) {
        case TT_SimpleExpr:
            printf("Simple Expr\n");
            break;
        case TT_AssignExpr:
            printf("Assign Expr\n");
            break;
        case TT_IfStmt:
            printf("If Stmt\n");
            break;
        case TT_WhileStmt:
            printf("While Stmt\n");
            break;
        case TT_ReturnStmt:
            printf("Return Stmt\n");
            break;
        case TT_FuncCall:
            printf("Func Call\n");
            break;
        default:
            break;
    }
}

void stp_func_decl(const DeerFuncDecl *fd, int deep)
{
    const DeerNode *n = nullptr;
    const DeerParam *p = nullptr;
    const DeerVarDecl *vd = nullptr;

    stp_print_space(deep);
    printf("FuncDecl: {\n");

    if (fd->vt != VT_UNDEFINE) {
        stp_print_space(deep + 1);
        printf("var type: %s\n", fd->vt == VT_INT ? "INT" : "VOID");
    }

    if (fd->fname) {
        stp_print_space(deep + 1);
        printf("function name: %s\n", fd->fname);
    }

    if (fd->params) {
        stp_print_space(deep + 1);
        printf("params: {\n");

        foreach (DeerParam, p, fd->params) {
            stp_print_param(p, deep + 1);
        }

        stp_print_space(deep + 1);
        printf("}\n");
    }

    if (fd->var_list) {
        stp_print_space(deep + 1);
        printf("var list: {\n");

        foreach (DeerVarDecl, vd, fd->var_list) {
            stp_var_decl(vd, deep + 2);
        }

        stp_print_space(deep + 1);
        printf("}\n");
    }

    if (fd->stmt_list) {
        stp_print_space(deep + 1);
        printf("stmt list: {\n");

        foreach (DeerNode, n, fd->stmt_list) {
            stp_raw_stmt(n, deep + 2);
        }

        stp_print_space(deep + 1);
        printf("}\n");
    }

    stp_print_space(deep);
    printf("}\n");
}


void syntax_tree_print(DeerDeclList *ast)
{
    int deep = 0;
    const DeerNode *node = nullptr;
    DeerLinkedList *list = nullptr;
    if (!ast || ast->type != TT_DeclList || (list = ast->decls) == nullptr) {
        return;
    }

    foreach (DeerNode, node, list) {
        switch (node->type) {
            case TT_VarDecl:
                stp_var_decl((DeerVarDecl *)node, deep);
                break;
            case TT_FuncDecl:
                stp_func_decl((DeerFuncDecl *)node, deep);
                break;
            default:
                break;
        }
        printf("\n");
    }
}

////////////////////////////////////////////////////////////////////////////////
// extern
////////////////////////////////////////////////////////////////////////////////
extern char *optarg;

////////////////////////////////////////////////////////////////////////////////
// function
////////////////////////////////////////////////////////////////////////////////
static void generate_asm(DeerCompilerHandle *handle)
{
    if (file_read_content(handle->cmm_file, &handle->file_content)) {
        invalid_call("file read content");
    }

    if (lexical_analysis(handle)) {
        invalid_call("DeerLexical analysis");
    }
    deer_token_list_print(handle->tokens);

    if (syntax_analysis(handle)) {
        invalid_call("syntax analysis");
    }
    syntax_tree_print(handle->ast);

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

static void execute_code(DeerCompilerHandle *handle)
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

    NEW(handle, DeerCompilerHandle);
    NEW(handle->lex, DeerLexical);
    NEW(handle->generator, CodeGenerator);

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