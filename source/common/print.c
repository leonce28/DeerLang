#include "common/print.h"

void token_list_print(DeerLinkedList *list) 
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

void symbol_space_print(const SymbolSpace *space)
{
    assert(space);
    const Symbol *symbol = nullptr;
    printf("SymbolSpace: %s { \n", space->name);

    foreach (Symbol, symbol, space->symbols) {
        printf("  name: %s, idx: %d, size: %d\n", symbol->name, symbol->idx, symbol->size);
    }

    printf("} \n\n");
}


void symbol_table_print(const SymbolTable *table)
{
    assert(table);
    const SymbolSpace *space = nullptr;

    symbol_space_print(table->global);
    symbol_space_print(table->access);

    foreach (SymbolSpace, space, table->spaces) {
        symbol_space_print(space);
    }
}

void code_maps_print(DeerLinkedList *maps)
{
    assert(maps);
    const Code *code = nullptr;
    const CodeMap *map = nullptr;

    foreach (CodeMap, map, maps) {
        printf("CodeMap: %s { \n", map->name);
        
        foreach (Code, code, map->codes) {
            printf("  %2d: %s\n", code->ins, code->offset);
        }

        printf("} \n\n");
    }
}

void code_list_print(DeerLinkedList *codes)
{
    assert(codes);
    const Code *code = nullptr;

    printf("CodeList: { \n");

    foreach (Code, code, codes) {
        printf("  %2d: %s\n", code->ins, code->offset);
    }

    printf("} \n\n");
}

void func_jump_print(DeerLinkedList *jumps)
{
    assert(jumps);
    const FuncJump *jump = nullptr;

    foreach (FuncJump, jump, jumps) {
        printf("FuncJump: [%s, %d] \n", jump->name, jump->start);
    }
}

void token_print(const DeerToken *token)
{
    assert(token);
    printf("line_no: %d, token_str: \"%s\", token_len: %d, type: %d\n",
            token->line_no, token->token_str, token->token_len, token->type);
}
