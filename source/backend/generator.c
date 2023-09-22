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

// void generate_if_stmt_code(CodeGenerator *generator)
// { 
//     int cl_idx, if_size, else_size;
//     DeerAST *node = generator->node;

//     // if_stmt :: = if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]

//     generator->node = node->sub_list[0];
//     gcode_simple_expr(generator);

//     if (!node->sub_list[2]) {
//         if_size = generator->cl->c_idx;
//         cl_idx = code_list_push(generator->cl, INS_JZ, NULL_STRING);
//         generator->node = node->sub_list[1];

//         gcode_stmt_list(generator);
//         if_size = generator->cl->c_idx - if_size;
        
//         snprintf(generator->size, VAR_SIZE_MAX, "%d", if_size);
//         code_list_set(generator->cl, cl_idx, generator->size);
//     } else {

//         /* if begin */
//         if_size = generator->cl->c_idx;
//         cl_idx = code_list_push(generator->cl, INS_JZ, NULL_STRING);
//         generator->node = node->sub_list[1];

//         gcode_stmt_list(generator);
//         if_size = generator->cl->c_idx - if_size;

//         snprintf(generator->size, VAR_SIZE_MAX, "%d", if_size + 1); // else JMP INSTRUCTION
//         code_list_set(generator->cl, cl_idx, generator->size);
//         /* if end */

//         /* else begin */
//         else_size = generator->cl->c_idx;
//         cl_idx = code_list_push(generator->cl, INS_JMP, NULL_STRING);
//         generator->node = node->sub_list[2];

//         gcode_stmt_list(generator);
//         else_size = generator->cl->c_idx - else_size;

//         snprintf(generator->size, VAR_SIZE_MAX, "%d", else_size);
//         code_list_set(generator->cl, cl_idx, generator->size);
//         /* else end */
//     }
// }

// void generate_while_stmt_code(CodeGenerator *generator)
// { 
//     DeerAST *node = generator->node;

//     // while_stmt ::= while '(' expr ')' stmt_list
//     generator->node = node->sub_list[0];
//     gcode_simple_expr(generator);

//     snprintf(generator->size, VAR_SIZE_MAX, "%d", node->sub_list[1]->sub_idx + 1);
//     code_list_push(generator->cl, INS_JZ, generator->size);

//     generator->node = node->sub_list[1];
//     gcode_stmt_list(generator);

//     snprintf(generator->size, VAR_SIZE_MAX, "-%d", node->sub_list[0]->sub_idx - 1);
//     code_list_push(generator->cl, INS_JMP, generator->size);
// }

// void generate_return_stmt_code(CodeGenerator *generator)
// { 
//     DeerAST *node = generator->node;

//     // return_stmt ::= return [ expr ] ';'
//     if (node->sub_list[0]) {
//         generator->node = node->sub_list[0];
//         gcode_simple_expr(generator);
//     }
// }

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
                // gcode_if_stmt(generator);
                break;
            case TT_WhileStmt:
                // gcode_while_stmt(generator);
                break;
            case TT_ReturnStmt:
                // gcode_return_stmt(generator);
                break;
            case TT_FuncCall:
                // gcode_return_stmt(generator);
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
    // int idx;
    const DeerNode *node = nullptr;

    generate_codes_global(handle);
    generate_codes_access(handle);

    handle->maps = dlist_push_back(handle->maps, create_code_map(NAMESPACE_GLOBAL, handle->codes));

    // declared_list ::= declared { declared }
    foreach (DeerNode, node, handle->ast->decls) {

        // declared ::= local_declared | func_declared
        if (node->type == TT_VarDecl) {
            continue;
        }

        handle->codes = nullptr;
        handle->space = ((DeerFuncDecl *)node)->fname;

        // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
        gcode_stmt_list(((DeerFuncDecl *)node)->stmt_list, handle);

        if (strcmp(NAMESPACE_ACCESS, handle->space) != 0) {
            handle->codes = dlist_push_back(handle->codes, create_code(INS_RET, nullptr));
        } 

        handle->maps = dlist_push_back(handle->maps, create_code_map(handle->space, handle->codes));
    }

    return CMM_SUCCESS;
}

// int _merge_code_map(CodeGenerator *generator)
// {
//     int idx, jump_num;
//     char *func_name = NULL;
//     map_list *maps = NULL;
//     code_list *codes = NULL;

//     // global code
//     maps = find_map_list(generator->c_map, NAMESPACE_GLOBAL);
//     if (!maps || !maps->cl) {
//         invalid_call("find_map_list global failed in merge_code_map.");
//     }
//     jump_num = maps->cl->c_idx;
//     code_list_append(generator->codes, maps->cl);

//     // other functions
//     for (idx = 0; idx < generator->c_map->m_idx; ++idx) {
//         codes = generator->c_map->maps[idx]->cl;
//         func_name = generator->c_map->maps[idx]->name;
//         if (strcmp(func_name, NAMESPACE_GLOBAL) == 0 || 
//             strcmp(func_name, NAMESPACE_ACCESS) == 0) {
//             continue;
//         }
//         code_list_append(generator->codes, codes);
//         set_func_jump_map(generator->jumps, func_name, jump_num);
//         jump_num += codes->c_idx;

//     }

//     // the "main" function must be the last function
//     maps = find_map_list(generator->c_map, NAMESPACE_ACCESS);
//     if (!maps || !maps->cl) {
//         invalid_call("find_map_list main failed in merge_code_map.");
//     }
//     code_list_append(generator->codes, maps->cl);
//     set_func_jump_map(generator->jumps, NAMESPACE_ACCESS, jump_num);

//     return CMM_SUCCESS;
// }

// int _translate_call(CodeGenerator *generator)
// {
//     int IP, offset;

//     // A virtual "IP"
//     for (IP = 0; IP < generator->codes->c_idx; ++IP) {
//         if (generator->codes->c[IP]->ins == INS_CALL) {
//             offset = get_func_jump_num(generator->jumps, generator->codes->c[IP]->offset) - IP;
//             snprintf(generator->codes->c[IP]->offset, VAR_OFFSET_MAX, "%d", offset);
//         }
//     }

//     return CMM_SUCCESS;
// }

int generate_code(DeerCompilerHandle *handle)
{
    // handle->generator->tree = handle->ast;
    // handle->generator->table = handle->table;

    if (generate_code_map(handle)) {
        invalid_call("create code map");
    }
    code_maps_print(handle->maps);

    // if (_merge_code_map(handle->generator)) {
    //     invalid_call("merge code map");
    // }
    // // code_list_print(generator->codes);
    // // func_jump_map_print(generator->jumps);

    // if (_translate_call(handle->generator)) {
    //     invalid_call("translate call");
    // }

    // handle->codes = handle->generator->codes;
    return CMM_SUCCESS;
}
