#include <string.h>
#include <assert.h>
#include "backend/generator.h"
#include "common/funcs.h"

void gcode_simple_expr(const DeerSimpleExpr *expr, DeerCompilerHandle *handle);
void gcode_stmt_list(const DeerLinkedList *stmt_list, DeerCompilerHandle *handle);

void gcode_number(DeerNumber *number, DeerCompilerHandle *handle)
{
    handle->codes = dlist_push_back(handle->codes, create_code(INS_LDC, nullptr));
}

void gcode_arg_list(const DeerLinkedList *arg_list, DeerCompilerHandle *handle)
{
    assert(arg_list && handle);
    const DeerSimpleExpr *simple = nullptr;

    // arg_list ::= expr { ',' expr }
    foreach (DeerSimpleExpr, simple, arg_list) {
        gcode_simple_expr(simple, handle);
        handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
    }
}

void gcode_func_call(const DeerFuncCall *func, DeerCompilerHandle *handle)
{
    // call ::= id '(' [ arg_list ] ')'

    int local_size = 0;
    int idx = 0, a_idx = 0;
    const Symbol *symbol = nullptr;
    SymbolSpace *reverse = nullptr;
    const SymbolSpace *space = nullptr;
    char offset[VAR_SIZE_MAX] = { 0 };

    // xxx = input();
    if (strcmp(func->fname, "input") == 0) {
        handle->codes = dlist_push_back(handle->codes, create_code(INS_IN, nullptr));
        return ;
    }

    // output(xxx);
    if (strcmp(func->fname, "output") == 0) {
        if (func->args) {
            gcode_arg_list(func->args, handle);
        }
        handle->codes = dlist_push_back(handle->codes, create_code(INS_OUT, nullptr));
        return ;
    }

    // user define function.
    space = find_symbol_space(handle->table, func->fname);
    assert(space);

    // reverse symbols to: ..., Local5, Local4, Local3, Param2, Param1, Param0
    reverse = create_symbol_space(func->fname);
    reverse->symbols = dlist_reverse_copy(space->symbols, sizeof(Symbol));

    // symbol_space_print(reverse);

    // We only need local variable here
    if (reverse->symbols) {
        local_size = reverse->symbols->size - (func->args ? func->args->size : 0);
    }

    // Push local variable
    foreach (Symbol, symbol, reverse->symbols) {
        if (idx++ >= local_size) {
            break;
        }

        // Scalar
        if (symbol->size == 0) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
            continue;
        }

        // Array, Push array content (by array size times)
        for (a_idx = 0; a_idx < symbol->size; ++a_idx) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
        }
        snprintf(offset, VAR_SIZE_MAX, "%d", symbol->size);
        handle->codes = dlist_push_back(handle->codes, create_code(INS_ADDR, offset));
        handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
    }

    // Push parameter
    if (func->args) {
        gcode_arg_list(func->args, handle);
    }
    handle->codes = dlist_push_back(handle->codes, create_code(INS_CALL, func->fname));

    // After call, we need several "POP" to pop all variables.
    foreach (Symbol, symbol, space->symbols) {
        // Any variable needs a "POP"
        handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, nullptr));

        // Pop array content (by array size times)
        for (a_idx = 0; a_idx < symbol->size; ++a_idx) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, nullptr));
        }
    }
}

void gcode_var(const DeerVar *var, DeerCompilerHandle *handle)
{
    // var ::= id [ '[' expr ']' ]
    bool is_global = false;
    const Symbol *symbol = nullptr;
    char offset[VAR_SIZE_MAX] = { 0 };

    // Is local var or global ? INS_LD or INS_ALD.
    if ((symbol = find_symbol(handle->table, handle->space, var->id)) == nullptr) {
        is_global = true;
        symbol = find_symbol_global(handle->table, var->id);
    }

    assert(symbol);

    snprintf(offset, VAR_SIZE_MAX, "%d", symbol->idx);
    handle->codes = dlist_push_back(handle->codes, create_code(INS_LDC, offset));
    handle->codes = dlist_push_back(handle->codes, create_code(is_global ? INS_LD : INS_ALD, nullptr));

    // Array
    if (symbol->idx) {
        handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, offset));
        
        gcode_simple_expr(var->index, handle);

        handle->codes = dlist_push_back(handle->codes, create_code(INS_ADD, offset));
        handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, offset));
        handle->codes = dlist_push_back(handle->codes, create_code(INS_ALD, offset));
    }
}

void gcode_factor(const DeerNode *node, DeerCompilerHandle *handle)
{
    assert(node && handle);

    // factor ::= '(' expr ')' | var | call | num
    switch (node->type) {
        case TT_SimpleExpr:
            gcode_simple_expr((DeerSimpleExpr *)node, handle);
            break;
        case TT_Number:
            gcode_number((DeerNumber *)node, handle);
            break;
        case TT_FuncCall:
            gcode_func_call((DeerFuncCall *)node, handle);
            break;
        case TT_Var:
            gcode_var((DeerVar *)node, handle);
            break;
        default:
            invalid_node(node);
            break;
    }
}

void gcode_operate_type(OperateType type, DeerCompilerHandle *handle)
{
    switch (type) {
        // rel_op ::= '<' | '<' '=' | '>' | '>' '=' | '=' '=' | '!' '='
        case OT_LESS:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_LT, nullptr));
            break;
        case OT_LESS_EQ:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_LE, nullptr));
            break;
        case OT_GREATER:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_GT, nullptr));
            break;
        case OT_GREATER_EQ:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_GE, nullptr));
            break;
        case OT_EQUAL:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_EQ, nullptr));
            break;
        case OT_NOT_EQ:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_NE, nullptr));
            break;

        // add_op ::= '+' | '-'
        case OT_PLUS:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_ADD, nullptr));
            break;
        case OT_MINUS:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_SUB, nullptr));
            break;

        // mul_op ::= '*' | '/')
        case OT_MULTIPLY:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_MUL, nullptr));
            break;
        case OT_DIVIDE:
            handle->codes = dlist_push_back(handle->codes, create_code(INS_DIV, nullptr));
            break;
        
        // invalid type
        default:
            assert(0);
            break;
    }
}

void gcode_simple_expr(const DeerSimpleExpr *simple, DeerCompilerHandle *handle)
{
    assert(handle);

    if (!simple) {
        return ;
    }

    // var | call | num
    if (simple->type != TT_SimpleExpr) {
        gcode_factor((DeerNode *)simple, handle);
        return ;
    }

    // simple_expr ::= add_expr { rel_op add_expr }
    // add_expr ::= term { add_op term }
    // term ::= factor { mul_op factor }
    // factor ::= '(' expr ')' | var | call | num
    switch (simple->op) {
        case OT_UNDEFINE:
            gcode_factor(simple->lchild, handle);
            break;
        case OT_LESS:
        case OT_LESS_EQ:
        case OT_GREATER:
        case OT_GREATER_EQ:
        case OT_EQUAL:
        case OT_NOT_EQ:
        case OT_PLUS:
        case OT_MINUS:
        case OT_MULTIPLY:
        case OT_DIVIDE:
            gcode_simple_expr((DeerSimpleExpr *)simple->lchild, handle);
            handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
            gcode_simple_expr((DeerSimpleExpr *)simple->rchild, handle);
            gcode_operate_type(simple->op, handle);
            handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, nullptr));
            break;
        default:
            break;
    }
}

void gcode_assign_expr(const DeerAssignExpr *assign, DeerCompilerHandle *handle)
{
    assert(assign && handle);

    bool is_global = false;
    const Symbol *symbol = nullptr;
    char offset[VAR_SIZE_MAX] = { 0 };

    gcode_simple_expr(assign->expr, handle);

    // var ::= id [ '[' expr ']' ]
    handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));

    // check is local var in current function ?
    symbol = find_symbol(handle->table, handle->space, assign->var->id);

    // maybe global var ?
    if (!symbol) {
        is_global = 1;
        symbol = find_symbol_global(handle->table, assign->var->id);
    }

    assert(symbol);

    snprintf(offset, VAR_SIZE_MAX, "%d", symbol->idx);
    handle->codes = dlist_push_back(handle->codes, create_code(INS_LDC, offset));

    // is Scalar ?
    if (!symbol->idx) {
        handle->codes = dlist_push_back(handle->codes, create_code(is_global ? INS_AST : INS_ST, nullptr));
        handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, nullptr));
        return ;
    }

    // Array, Get the (start) pointer (is already an absolute address)
    handle->codes = dlist_push_back(handle->codes, create_code(is_global ? INS_ALD : INS_LD, nullptr));
    handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));

    gcode_var(assign->var, handle);

    // Pointer[Index] (Pointer + Index)
    handle->codes = dlist_push_back(handle->codes, create_code(INS_ADD, nullptr));
    handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, nullptr));

    // Save by absolute address
    handle->codes = dlist_push_back(handle->codes, create_code(INS_AST, nullptr));

    handle->codes = dlist_push_back(handle->codes, create_code(INS_POP, nullptr));
}

void gcode_if_stmt(const DeerIfStmt *if_stmt, DeerCompilerHandle *handle)
{ 
    assert(if_stmt && handle);

    Code *code = nullptr;
    char offset[VAR_SIZE_MAX] = { 0 };
    int index, if_size, else_size;

    // if_stmt :: = if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]
    gcode_simple_expr(if_stmt->condition, handle);


    // if stmt
    code = create_code(INS_JZ, nullptr);
    handle->codes = dlist_push_back(handle->codes, code);
    index = handle->codes->size;

    gcode_stmt_list(if_stmt->if_block, handle);

    if_size = handle->codes->size - index;

    if (if_stmt->else_block) {
        if_size += 1;   // else JMP INSTRUCTION
    }

    snprintf(offset, VAR_SIZE_MAX, "%d", if_size); 
    strncpy(code->offset, offset, VAR_OFFSET_MAX);

    // no else stmt
    if (!if_stmt->else_block) {
        handle->codes = dlist_push_back(handle->codes, create_code(INS_JZ, nullptr));
        return ;
    }

    // else stmt
    code = create_code(INS_JMP, nullptr);
    handle->codes = dlist_push_back(handle->codes, code);
    index = handle->codes->size;

    gcode_stmt_list(if_stmt->else_block, handle);

    else_size = handle->codes->size - index;

    snprintf(offset, VAR_SIZE_MAX, "%d", else_size); 
    strncpy(code->offset, offset, VAR_OFFSET_MAX);
}

void gcode_while_stmt(const DeerWhileStmt *while_stmt, DeerCompilerHandle *handle)
{ 
    assert(while_stmt && handle);

    int index = 0;
    char offset[VAR_SIZE_MAX] = { 0 };

    // while_stmt ::= while '(' expr ')' stmt_list

    // condition
    index = handle->codes->size;
    gcode_simple_expr(while_stmt->condition, handle);

    snprintf(offset, VAR_SIZE_MAX, "%d", while_stmt->block->size + 1);
    handle->codes = dlist_push_back(handle->codes, create_code(INS_JZ, offset));

    // block
    gcode_stmt_list(while_stmt->block, handle);

    // jump to condition
    snprintf(offset, VAR_SIZE_MAX, "-%d", handle->codes->size - index);
    handle->codes = dlist_push_back(handle->codes, create_code(INS_JMP, offset));
}

void gcode_return_stmt(const DeerReturnStmt *return_stmt, DeerCompilerHandle *handle)
{ 
    assert(return_stmt && handle);

    // return_stmt ::= return [ expr ] ';'
    gcode_simple_expr(return_stmt->expr, handle);
}

void gcode_stmt_list(const DeerLinkedList *stmt_list, DeerCompilerHandle *handle)
{
    assert(stmt_list && handle);

    const DeerNode *node = nullptr;

    // stmt_list ::= { simple_expr | assign_expr | if_stmt | 
    //                 while_stmt | return_stmt }
    foreach (DeerNode, node, stmt_list) {
        switch (node->type) {
            case TT_SimpleExpr:
                gcode_simple_expr((DeerSimpleExpr *)node, handle);
                break;
            case TT_AssignExpr:
                gcode_assign_expr((const DeerAssignExpr *)node, handle);
                break;
            case TT_IfStmt:
                gcode_if_stmt((const DeerIfStmt *)node, handle);
                break;
            case TT_WhileStmt:
                gcode_while_stmt((const DeerWhileStmt *)node, handle);
                break;
            case TT_ReturnStmt:
                gcode_return_stmt((const DeerReturnStmt *)node, handle);
                break;
            case TT_FuncCall:
                gcode_func_call((const DeerFuncCall *)node, handle);
                break;
            default:
                invalid_node(node);
                break;
        }
    }
}

void generate_codes_global(DeerCompilerHandle *handle)
{
    assert(handle && handle->table && handle->table->global);

    int idx;
    const Symbol *symbol = nullptr;
    char offset[VAR_SIZE_MAX] = { 0 };
    const SymbolSpace *global = handle->table->global;

    foreach (Symbol, symbol, global->symbols) {
        // Array
        if (symbol->size) {
            // Calc the array start address (variable number + 1)
            snprintf(offset, VAR_SIZE_MAX, "%d", symbol->idx + 1);
            handle->codes = dlist_push_back(handle->codes, create_code(INS_LDC, offset));
        }

        // Push the array start address
        // (Or only a meaningless int for global scalar memeory)
        handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));

        // Push array content (by array size times)
        for (idx = 0; idx < symbol->size; ++idx) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
        }
    }
}

void generate_codes_access(DeerCompilerHandle *handle)
{
    assert(handle && handle->table && handle->table->access);

    const Symbol *symbol = nullptr;
    char offset[VAR_SIZE_MAX] = { 0 };
    const SymbolSpace *access = handle->table->access;

    foreach (Symbol, symbol, access->symbols) {
        if (symbol->size) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_LDC, nullptr));

            snprintf(offset, VAR_SIZE_MAX, "%d", symbol->idx);
            handle->codes = dlist_push_back(handle->codes, create_code(INS_ADDR, offset));

            handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
        } else {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_PUSH, nullptr));
        }
    }

    // Call the "main" function automatically
    handle->codes = dlist_push_back(handle->codes, create_code(INS_CALL, NAMESPACE_ACCESS));
}

int generate_code_map(DeerCompilerHandle *handle)
{
    const DeerNode *node = nullptr;

    generate_codes_global(handle);
    generate_codes_access(handle);

    handle->maps = dlist_push_back(handle->maps, create_code_map(NAMESPACE_GLOBAL, handle->codes));
    handle->codes = nullptr;

    // declared_list ::= declared { declared }
    foreach (DeerNode, node, handle->ast->decls) {

        // declared ::= local_declared | func_declared
        if (node->type == TT_VarDecl) {
            continue;
        }

        handle->space = ((DeerFuncDecl *)node)->fname;

        // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
        gcode_stmt_list(((DeerFuncDecl *)node)->stmt_list, handle);

        if (strcmp(NAMESPACE_ACCESS, handle->space) != 0) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_RET, nullptr));
        } 

        handle->maps = dlist_push_back(handle->maps, create_code_map(handle->space, handle->codes));
        handle->codes = nullptr;
    }

    return CMM_SUCCESS;
}

int generate_jump_map(DeerCompilerHandle *handle)
{
    assert(handle && handle->maps);

    int jump = 0;
    CodeMap *map = nullptr;

    // global map
    map = find_code_map(handle->maps, NAMESPACE_GLOBAL);

    assert(map && map->codes);

    jump = map->codes->size;

    handle->codes = dlist_append_merge(handle->codes, map->codes, sizeof(Code));

    // other functions
    foreach (CodeMap, map, handle->maps) {
        assert(map && map->codes);
        if (strcmp(map->name, NAMESPACE_GLOBAL) == 0 || 
            strcmp(map->name, NAMESPACE_ACCESS) == 0) {
            continue;
        }

        handle->codes = dlist_append_merge(handle->codes, map->codes, sizeof(Code));
        handle->jumps = dlist_push_back(handle->jumps, create_func_jump(map->name, jump));

        jump += map->codes->size;
    }

    // the "main" function must be the last function
    map = find_code_map(handle->maps, NAMESPACE_ACCESS);
    assert(map && map->codes);

    handle->codes = dlist_append_merge(handle->codes, map->codes, sizeof(Code));
    handle->jumps = dlist_push_back(handle->jumps, create_func_jump(map->name, jump));

    return CMM_SUCCESS;
}

int generate_func_call(DeerCompilerHandle *handle)
{
    assert(handle);

    int IP = 0;
    Code *code = nullptr;
    FuncJump *jump = nullptr;

    // A virtual "IP"
    foreach (Code, code, handle->codes) {
        if (code->ins == INS_CALL) {
            jump = find_func_jump(handle->jumps, code->offset);
            assert(jump);

            snprintf(code->offset, VAR_OFFSET_MAX, "%d", jump->start - IP);
        }

        ++IP;
    }

    return CMM_SUCCESS;
}

int generate_code(DeerCompilerHandle *handle)
{
    if (generate_code_map(handle)) {
        invalid_call("generate code map");
    }
    // code_maps_print(handle->maps);

    if (generate_jump_map(handle)) {
        invalid_call("generate jump map");
    }
    // func_jump_print(handle->jumps);

    if (generate_func_call(handle)) {
        invalid_call("generate func call");
    }

    return CMM_SUCCESS;
}
