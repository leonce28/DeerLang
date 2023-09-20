#include <string.h>
#include <assert.h>
#include "backend/generator.h"
#include "common/funcs.h"

// void generate_expr_code(CodeGenerator *generator);
// void generate_stmt_list_code(CodeGenerator *generator);

// void generate_number_code(CodeGenerator *generator)
// {
//     code_list_push(generator->cl, INS_LDC, generator->node->data->token_str);
// }

// void generate_arg_list_code(CodeGenerator *generator)
// {
//     // arg_list ::= expr { ',' expr }
//     int idx;
//     DeerAST *node = generator->node;

//     for (idx = node->sub_idx - 1; idx >= 0; --idx) {
//         generator->node = node->sub_list[idx];
//         generate_expr_code(generator);
//         code_list_push(generator->cl, INS_PUSH, NULL_STRING);
//     }
// }

// void generate_call_code(CodeGenerator *generator)
// {
//     // call ::= id '(' [ arg_list ] ')'
//     int idx, p_idx, top_idx, arr_idx;
//     symbol_space *space = NULL;
//     symbol_space *pairs = NULL;
//     DeerAST *node = generator->node;
//     char *func_name = node->sub_list[0]->data->token_str;

//     // xxx = input();
//     if (strcmp(func_name, "input") == 0) {
//         code_list_push(generator->cl, INS_IN, NULL_STRING);
//         return;
//     }
//     // output(xxx);
//     else if (strcmp(func_name, "output") == 0) {
//         generator->node = node->sub_list[1]->sub_list[0];
//         generate_expr_code(generator);

//         code_list_push(generator->cl, INS_OUT, NULL_STRING);
//         return;
//     }

//     // user define function.
//     space = find_symbol_space(generator->table, func_name);
//     pairs = create_symbol_space(func_name);
//     init_space_symbol(pairs, space->s_idx);

//     // ..., Local5, Local4, Local3, Param2, Param1, Param0
//     for (idx = 0; idx < space->s_idx; ++idx) {
//         p_idx = pairs->s_idx - space->s[idx]->var_idx - 1;
//         strncpy(pairs->s[p_idx]->var_name, space->s[idx]->var_name, MIN(VAR_NAME_MAX, strlen(space->s[idx]->var_name)));
//         pairs->s[p_idx]->var_idx = space->s[idx]->var_idx;
//         pairs->s[p_idx]->var_size = space->s[idx]->var_size;
//     }

//     // We only need local variable here
//     top_idx = pairs->s_idx - (node->sub_list[1] ? node->sub_list[1]->sub_idx : 0);

//     // Push local variable
//     for (idx = 0; idx < top_idx; ++idx) {
//         if (pairs->s[idx]->var_size) {
//             // Array, Push array content (by array size times)
//             for (arr_idx = 0; arr_idx < pairs->s[idx]->var_size; ++arr_idx) {
//                 code_list_push(generator->cl, INS_PUSH, NULL_STRING);
//             }
//             snprintf(generator->size, VAR_SIZE_MAX, "%d", pairs->s[idx]->var_size);
//             code_list_push(generator->cl, INS_ADDR, generator->size);
//             code_list_push(generator->cl, INS_PUSH, NULL_STRING);
//         } else {
//             // Scalar
//             code_list_push(generator->cl, INS_PUSH, NULL_STRING);
//         }
//     }

//     // Push parameter
//     if (node->sub_list[1]) {
//         generator->node = node->sub_list[1];
//         generate_arg_list_code(generator);
//     }

//     code_list_push(generator->cl, INS_CALL, func_name);

//     // After call, we need several "POP" to pop all variables.
//     for (idx = 0; idx < space->s_idx; ++idx) {
//         // Any variable needs a "POP"
//         code_list_push(generator->cl, INS_POP, NULL_STRING);

//         // Pop array content (by array size times)
//         for (arr_idx = 0; arr_idx < space->s[idx]->var_size; ++arr_idx) {
//             code_list_push(generator->cl, INS_POP, NULL_STRING);
//         }
//     }
// }

// void generate_var_code(CodeGenerator *generator)
// {
//     // var ::= id [ '[' expr ']' ]
//     symbol *sb = NULL;
//     DeerAST *node = generator->node;

//     // is local var in current function ?
//     sb = find_symbol(generator->table, generator->cur_space, node->sub_list[0]->data->token_str);

//     if (sb) {
//         // local var
//         snprintf(generator->size, VAR_SIZE_MAX, "%d", sb->var_idx);
//         code_list_push(generator->cl, INS_LDC, generator->size);
//         code_list_push(generator->cl, INS_LD, NULL_STRING);
//     } else {
//         // global var
//         sb = find_symbol(generator->table, NAMESPACE_GLOBAL, node->sub_list[0]->data->token_str);
//         assert(sb != NULL);

//         snprintf(generator->size, VAR_SIZE_MAX, "%d", sb->var_idx);
//         code_list_push(generator->cl, INS_LDC, generator->size);
//         code_list_push(generator->cl, INS_ALD, NULL_STRING);
//     }

//     // array
//     if (node->sub_list[1]) {
//         code_list_push(generator->cl, INS_PUSH, NULL_STRING);
        
//         generator->node = node->sub_list[1];
//         generate_expr_code(generator);

//         code_list_push(generator->cl, INS_ADD, NULL_STRING);
//         code_list_push(generator->cl, INS_POP, NULL_STRING);
//         code_list_push(generator->cl, INS_ALD, NULL_STRING);
//     }
// }

// void generate_factor_code(CodeGenerator *generator)
// {
//     // factor ::= '(' expr ')' | var | call | num
//     switch (generator->node->data->type) {
//         case TOKEN_EXPR:
//             generate_expr_code(generator);
//             break;
//         case TOKEN_NUMBER:
//             generate_number_code(generator);
//             break;
//         case TOKEN_CALL:
//             generate_call_code(generator);
//             break;
//         case TOKEN_VAR:
//             generate_var_code(generator);
//             break;
//         default:
//             invalid_token(generator->node->data);
//     }
// }

// void generate_mul_op_code(CodeGenerator *generator)
// {
//     // mul_op ::= '*' | '/')
//     if (TOKEN_TYPE_MATCH(generator->node, TOKEN_MULTIPLY)) {
//         code_list_push(generator->cl, INS_MUL, NULL_STRING);
//     } else {
//         code_list_push(generator->cl, INS_DIV, NULL_STRING);
//     }
// }

// void generate_term_code(CodeGenerator *generator)
// {
//     int idx;
//     DeerAST *node = generator->node;

//     // term ::= factor { mul_op factor }
//     generator->node = node->sub_list[0];
//     generate_factor_code(generator);

//     for (idx = 1; idx < node->sub_idx; idx += 2) {
//         code_list_push(generator->cl, INS_PUSH, NULL_STRING);

//         generator->node = node->sub_list[idx + 1];
//         generate_factor_code(generator);

//         generator->node = node->sub_list[idx];
//         generate_mul_op_code(generator);

//         code_list_push(generator->cl, INS_POP, NULL_STRING);
//     }
// }

// void generate_add_op_code(CodeGenerator *generator)
// {
//     // add_op ::= '+' | '-'
//     if (TOKEN_TYPE_MATCH(generator->node, TOKEN_PLUS)) {
//         code_list_push(generator->cl, INS_ADD, NULL_STRING);
//     } else {
//         code_list_push(generator->cl, INS_SUB, NULL_STRING);
//     }
// }

// void generate_add_expr_code(CodeGenerator *generator)
// {
//     int idx;
//     DeerAST *node = generator->node;

//     // add_expr ::= term { add_op term }
//     generator->node = node->sub_list[0];
//     generate_term_code(generator);

//     for (idx = 1; idx < node->sub_idx; idx += 2) {
//         code_list_push(generator->cl, INS_PUSH, NULL_STRING);

//         generator->node = node->sub_list[idx + 1];
//         generate_term_code(generator);

//         generator->node = node->sub_list[idx];
//         generate_add_op_code(generator);

//         code_list_push(generator->cl, INS_POP, NULL_STRING);
//     }
// }

// void generate_rel_op_code(CodeGenerator *generator)
// {
//     // rel_op ::= '<' | '<' '=' | '>' | '>' '=' | '=' '=' | '!' '='
//     switch (generator->node->data->type) {
//         case TOKEN_LESS:
//             code_list_push(generator->cl, INS_LT, NULL_STRING);
//             break;
//         case TOKEN_LESS_EQUAL:
//             code_list_push(generator->cl, INS_LE, NULL_STRING);
//             break;
//         case TOKEN_GREATER:
//             code_list_push(generator->cl, INS_GT, NULL_STRING);
//             break;
//         case TOKEN_GREATER_EQUAL:
//             code_list_push(generator->cl, INS_GE, NULL_STRING);
//             break;
//         case TOKEN_EQUAL:
//             code_list_push(generator->cl, INS_EQ, NULL_STRING);
//             break;
//         case TOKEN_NOT_EQUAL:
//             code_list_push(generator->cl, INS_NE, NULL_STRING);
//             break;
//         default:
//             invalid_token(generator->node->data);
//     }
// }

// void generate_simple_expr_code(CodeGenerator *generator)
// {
//     // simple_expr ::= add_expr [ rel_op add_expr ]

//     DeerAST *node = generator->node;

//     if (node == NULL) {
//         return;
//     }

//     if (!node->sub_list[1] && !node->sub_list[2]) {
//         generator->node = node->sub_list[0];
//         generate_add_expr_code(generator);
//     } else {
//         generator->node = node->sub_list[0];
//         generate_add_expr_code(generator);
    
//         code_list_push(generator->cl, INS_PUSH, NULL_STRING);

//         generator->node = node->sub_list[2];
//         generate_add_expr_code(generator);

//         generator->node = node->sub_list[1];
//         generate_rel_op_code(generator);

//         code_list_push(generator->cl, INS_POP, NULL_STRING);
//     }
// }

// void generate_assign_code(CodeGenerator *generator)
// {
//     int is_global = 0;
//     symbol *sb = NULL;
//     DeerAST *node = generator->node;

//     // var ::= id [ '[' expr ']' ]
//     code_list_push(generator->cl, INS_PUSH, NULL_STRING);

//     // is local var in current function ?
//     sb = find_symbol(generator->table, generator->cur_space, node->sub_list[0]->data->token_str);
    
//     // maybe global var ?
//     if (!sb) {
//         is_global = 1;
//         sb = find_symbol(generator->table, NAMESPACE_GLOBAL, node->sub_list[0]->data->token_str);
//     }
    
//     snprintf(generator->size, VAR_SIZE_MAX, "%d", sb->var_idx);
//     code_list_push(generator->cl, INS_LDC, generator->size);

//     // scalar or array
//     if (!node->sub_list[1]) {
//         code_list_push(generator->cl, is_global ? INS_AST : INS_ST, NULL_STRING);
//     } else {
//         // Get the (start) pointer (is already an absolute address)
//         code_list_push(generator->cl, is_global ? INS_ALD : INS_LD, NULL_STRING);
//         code_list_push(generator->cl, INS_PUSH, NULL_STRING);

//         generator->node = node->sub_list[0];
//         generate_expr_code(generator);

//         // Pointer[Index] (Pointer + Index)
//         code_list_push(generator->cl, INS_ADD, NULL_STRING);
//         code_list_push(generator->cl, INS_POP, NULL_STRING);

//         // Save by absolute address
//         code_list_push(generator->cl, INS_AST, NULL_STRING);
//     }

//     code_list_push(generator->cl, INS_POP, NULL_STRING);
// }

// void generate_expr_code(CodeGenerator *generator)
// {
//     DeerAST *node = generator->node;

//     /* expr_stmt ::= [ expr ] ';'
//      * expr ::= var '=' expr | simple_expr
//      *
//      * => expr_stmt ::= [ var '=' expr ] ';' | [ simple_expr ] ';'
//      */
//     if (!node->sub_list[1]) {
//         generator->node = node->sub_list[0];
//         generate_simple_expr_code(generator);
//     } else {
//         generator->node = node->sub_list[1];
//         generate_expr_code(generator);

//         generator->node = node->sub_list[0];
//         generate_assign_code(generator);
//     }
// }

// void generate_if_stmt_code(CodeGenerator *generator)
// { 
//     int cl_idx, if_size, else_size;
//     DeerAST *node = generator->node;

//     // if_stmt :: = if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]

//     generator->node = node->sub_list[0];
//     generate_expr_code(generator);

//     if (!node->sub_list[2]) {
//         if_size = generator->cl->c_idx;
//         cl_idx = code_list_push(generator->cl, INS_JZ, NULL_STRING);
//         generator->node = node->sub_list[1];

//         generate_stmt_list_code(generator);
//         if_size = generator->cl->c_idx - if_size;
        
//         snprintf(generator->size, VAR_SIZE_MAX, "%d", if_size);
//         code_list_set(generator->cl, cl_idx, generator->size);
//     } else {

//         /* if begin */
//         if_size = generator->cl->c_idx;
//         cl_idx = code_list_push(generator->cl, INS_JZ, NULL_STRING);
//         generator->node = node->sub_list[1];

//         generate_stmt_list_code(generator);
//         if_size = generator->cl->c_idx - if_size;

//         snprintf(generator->size, VAR_SIZE_MAX, "%d", if_size + 1); // else JMP INSTRUCTION
//         code_list_set(generator->cl, cl_idx, generator->size);
//         /* if end */

//         /* else begin */
//         else_size = generator->cl->c_idx;
//         cl_idx = code_list_push(generator->cl, INS_JMP, NULL_STRING);
//         generator->node = node->sub_list[2];

//         generate_stmt_list_code(generator);
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
//     generate_expr_code(generator);

//     snprintf(generator->size, VAR_SIZE_MAX, "%d", node->sub_list[1]->sub_idx + 1);
//     code_list_push(generator->cl, INS_JZ, generator->size);

//     generator->node = node->sub_list[1];
//     generate_stmt_list_code(generator);

//     snprintf(generator->size, VAR_SIZE_MAX, "-%d", node->sub_list[0]->sub_idx - 1);
//     code_list_push(generator->cl, INS_JMP, generator->size);
// }

// void generate_return_stmt_code(CodeGenerator *generator)
// { 
//     DeerAST *node = generator->node;

//     // return_stmt ::= return [ expr ] ';'
//     if (node->sub_list[0]) {
//         generator->node = node->sub_list[0];
//         generate_expr_code(generator);
//     }
// }

// void generate_stmt_code(CodeGenerator *generator)
// {
//     // stmt ::= expr_stmt | if_stmt | while_stmt | return_stmt
    
//     if (NULL == generator->node) {
//         return;
//     }

//     switch (generator->node->data->type) {
//         case TOKEN_EXPR:
//             generate_expr_code(generator);
//             break;
//         case TOKEN_IF_STMT:
//             generate_if_stmt_code(generator);
//             break;
//         case TOKEN_WHILE_STMT:
//             generate_while_stmt_code(generator);
//             break;
//         case TOKEN_RETURN_STMT:
//             generate_return_stmt_code(generator);
//             break;
//         default:
//             invalid_token(generator->node->data);
//     }
// }

void generate_stmt_list_code(CodeGenerator *generator)
{
    // int idx;
    // DeerAST *node;

    // // stmt_list ::= { stmt }
    // for (idx = 0, node = generator->node; idx < node->sub_idx; ++idx) {
    //     generator->node = node->sub_list[idx];
    //     generate_stmt_code(generator);
    // }
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

int _create_code_map(DeerCompilerHandle *handle)
{
    // int idx;
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

        // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
        // generator->cur_space = node->sub_list[1]->data->token_str;
        // generator->node = node->sub_list[4];
        // generate_stmt_list_code(generator);

        // if (strcmp(NAMESPACE_ACCESS, generator->cur_space) != 0) {
        //     code_list_push(generator->cl, INS_RET, NULL_STRING);
        // }

        // set_code_map(generator->c_map, generator->cur_space, generator->cl);

        handle->codes = nullptr;
    }

    // for (idx = 0; idx < handle->tree->sub_idx; ++idx) {
    //     node = generator->tree->sub_list[idx];

    //     // declared ::= local_declared | func_declared
    //     if (TOKEN_TYPE_MATCH(node, TOKEN_FUNC_DECL)) {

    //         // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
    //         generator->cur_space = node->sub_list[1]->data->token_str;
    //         generator->node = node->sub_list[4];
    //         generate_stmt_list_code(generator);

    //         if (strcmp(NAMESPACE_ACCESS, generator->cur_space) != 0) {
    //             code_list_push(generator->cl, INS_RET, NULL_STRING);
    //         }

    //         set_code_map(generator->c_map, generator->cur_space, generator->cl);
    //     }

    //     code_list_clean(generator->cl);
    // }

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

    if (_create_code_map(handle)) {
        invalid_call("create code map");
    }
    // code_map_print(generator->c_map);

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
