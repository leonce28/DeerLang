#include <string.h>
#include <assert.h>
#include "backend/code_generator.h"
#include "common/global_funcs.h"

void _generate_expr_code(code_generator_handler *cgh);
void _generate_stmt_list_code(code_generator_handler *cgh);

void _generate_number_code(code_generator_handler *cgh)
{
    code_list_push(cgh->cl, INS_LDC, cgh->node->data->token_str);
}

void _generate_arg_list_code(code_generator_handler *cgh)
{
    // arg_list ::= expr { ',' expr }
    int idx;
    syntax_tree *node = cgh->node;

    for (idx = node->sub_idx - 1; idx >= 0; --idx) {
        cgh->node = node->sub_list[idx];
        _generate_expr_code(cgh);
        code_list_push(cgh->cl, INS_PUSH, NULL_STRING);
    }
}

void _generate_call_code(code_generator_handler *cgh)
{
    // call ::= id '(' [ arg_list ] ')'
    int idx, p_idx, top_idx, arr_idx;
    symbol_space *space = NULL;
    symbol_space *pairs = NULL;
    syntax_tree *node = cgh->node;
    char *func_name = node->sub_list[0]->data->token_str;

    // xxx = input();
    if (strcmp(func_name, "input") == 0) {
        code_list_push(cgh->cl, INS_IN, NULL_STRING);
        return;
    }
    // output(xxx);
    else if (strcmp(func_name, "output") == 0) {
        cgh->node = node->sub_list[1]->sub_list[0];
        _generate_expr_code(cgh);

        code_list_push(cgh->cl, INS_OUT, NULL_STRING);
        return;
    }

    // user define function.
    space = find_symbol_space(cgh->table, func_name);
    pairs = create_symbol_space(func_name);
    init_space_symbol(pairs, space->s_idx);

    // ..., Local5, Local4, Local3, Param2, Param1, Param0
    for (idx = 0; idx < space->s_idx; ++idx) {
        p_idx = pairs->s_idx - space->s[idx]->var_idx - 1;
        strncpy(pairs->s[p_idx]->var_name, space->s[idx]->var_name, MIN(VAR_NAME_MAX, strlen(space->s[idx]->var_name)));
        pairs->s[p_idx]->var_idx = space->s[idx]->var_idx;
        pairs->s[p_idx]->var_size = space->s[idx]->var_size;
    }

    // We only need local variable here
    top_idx = pairs->s_idx - (node->sub_list[1] ? node->sub_list[1]->sub_idx : 0);

    // Push local variable
    for (idx = 0; idx < top_idx; ++idx) {
        if (pairs->s[idx]->var_size) {
            // Array, Push array content (by array size times)
            for (arr_idx = 0; arr_idx < pairs->s[idx]->var_size; ++arr_idx) {
                code_list_push(cgh->cl, INS_PUSH, NULL_STRING);
            }
            snprintf(cgh->size, VAR_SIZE_MAX, "%d", pairs->s[idx]->var_size);
            code_list_push(cgh->cl, INS_ADDR, cgh->size);
            code_list_push(cgh->cl, INS_PUSH, NULL_STRING);
        } else {
            // Scalar
            code_list_push(cgh->cl, INS_PUSH, NULL_STRING);
        }
    }

    // Push parameter
    if (node->sub_list[1]) {
        cgh->node = node->sub_list[1];
        _generate_arg_list_code(cgh);
    }

    code_list_push(cgh->cl, INS_CALL, func_name);

    // After call, we need several "POP" to pop all variables.
    for (idx = 0; idx < space->s_idx; ++idx) {
        // Any variable needs a "POP"
        code_list_push(cgh->cl, INS_POP, NULL_STRING);

        // Pop array content (by array size times)
        for (arr_idx = 0; arr_idx < space->s[idx]->var_size; ++arr_idx) {
            code_list_push(cgh->cl, INS_POP, NULL_STRING);
        }
    }
}

void _generate_var_code(code_generator_handler *cgh)
{
    // var ::= id [ '[' expr ']' ]
    symbol *sb = NULL;
    syntax_tree *node = cgh->node;

    // is local var in current function ?
    sb = find_symbol(cgh->table, cgh->cur_space, node->sub_list[0]->data->token_str);

    if (sb) {
        // local var
        snprintf(cgh->size, VAR_SIZE_MAX, "%d", sb->var_idx);
        code_list_push(cgh->cl, INS_LDC, cgh->size);
        code_list_push(cgh->cl, INS_LD, NULL_STRING);
    } else {
        // global var
        sb = find_symbol(cgh->table, NAMESPACE_GLOBAL, node->sub_list[0]->data->token_str);
        assert(sb != NULL);

        snprintf(cgh->size, VAR_SIZE_MAX, "%d", sb->var_idx);
        code_list_push(cgh->cl, INS_LDC, cgh->size);
        code_list_push(cgh->cl, INS_ALD, NULL_STRING);
    }

    // array
    if (node->sub_list[1]) {
        code_list_push(cgh->cl, INS_PUSH, NULL_STRING);
        
        cgh->node = node->sub_list[1];
        _generate_expr_code(cgh);

        code_list_push(cgh->cl, INS_ADD, NULL_STRING);
        code_list_push(cgh->cl, INS_POP, NULL_STRING);
        code_list_push(cgh->cl, INS_ALD, NULL_STRING);
    }
}

void _generate_factor_code(code_generator_handler *cgh)
{
    // factor ::= '(' expr ')' | var | call | num
    switch (cgh->node->data->token_type) {
        case TOKEN_EXPR:
            _generate_expr_code(cgh);
            break;
        case TOKEN_NUMBER:
            _generate_number_code(cgh);
            break;
        case TOKEN_CALL:
            _generate_call_code(cgh);
            break;
        case TOKEN_VAR:
            _generate_var_code(cgh);
            break;
        default:
            invalid_token(cgh->node->data);
    }
}

void _generate_mul_op_code(code_generator_handler *cgh)
{
    // mul_op ::= '*' | '/')
    if (TOKEN_TYPE_MATCH(cgh->node, TOKEN_MULTIPLY)) {
        code_list_push(cgh->cl, INS_MUL, NULL_STRING);
    } else {
        code_list_push(cgh->cl, INS_DIV, NULL_STRING);
    }
}

void _generate_term_code(code_generator_handler *cgh)
{
    int idx;
    syntax_tree *node = cgh->node;

    // term ::= factor { mul_op factor }
    cgh->node = node->sub_list[0];
    _generate_factor_code(cgh);

    for (idx = 1; idx < node->sub_idx; idx += 2) {
        code_list_push(cgh->cl, INS_PUSH, NULL_STRING);

        cgh->node = node->sub_list[idx + 1];
        _generate_factor_code(cgh);

        cgh->node = node->sub_list[idx];
        _generate_mul_op_code(cgh);

        code_list_push(cgh->cl, INS_POP, NULL_STRING);
    }
}

void _generate_add_op_code(code_generator_handler *cgh)
{
    // add_op ::= '+' | '-'
    if (TOKEN_TYPE_MATCH(cgh->node, TOKEN_PLUS)) {
        code_list_push(cgh->cl, INS_ADD, NULL_STRING);
    } else {
        code_list_push(cgh->cl, INS_SUB, NULL_STRING);
    }
}

void _generate_add_expr_code(code_generator_handler *cgh)
{
    int idx;
    syntax_tree *node = cgh->node;

    // add_expr ::= term { add_op term }
    cgh->node = node->sub_list[0];
    _generate_term_code(cgh);

    for (idx = 1; idx < node->sub_idx; idx += 2) {
        code_list_push(cgh->cl, INS_PUSH, NULL_STRING);

        cgh->node = node->sub_list[idx + 1];
        _generate_term_code(cgh);

        cgh->node = node->sub_list[idx];
        _generate_add_op_code(cgh);

        code_list_push(cgh->cl, INS_POP, NULL_STRING);
    }
}

void _generate_rel_op_code(code_generator_handler *cgh)
{
    // rel_op ::= '<' | '<' '=' | '>' | '>' '=' | '=' '=' | '!' '='
    switch (cgh->node->data->token_type) {
        case TOKEN_LESS:
            code_list_push(cgh->cl, INS_LT, NULL_STRING);
            break;
        case TOKEN_LESS_EQUAL:
            code_list_push(cgh->cl, INS_LE, NULL_STRING);
            break;
        case TOKEN_GREATER:
            code_list_push(cgh->cl, INS_GT, NULL_STRING);
            break;
        case TOKEN_GREATER_EQUAL:
            code_list_push(cgh->cl, INS_GE, NULL_STRING);
            break;
        case TOKEN_EQUAL:
            code_list_push(cgh->cl, INS_EQ, NULL_STRING);
            break;
        case TOKEN_NOT_EQUAL:
            code_list_push(cgh->cl, INS_NE, NULL_STRING);
            break;
        default:
            invalid_token(cgh->node->data);
    }
}

void _generate_simple_expr_code(code_generator_handler *cgh)
{
    // simple_expr ::= add_expr [ rel_op add_expr ]

    syntax_tree *node = cgh->node;

    if (node == NULL) {
        return;
    }

    if (!node->sub_list[1] && !node->sub_list[2]) {
        cgh->node = node->sub_list[0];
        _generate_add_expr_code(cgh);
    } else {
        cgh->node = node->sub_list[0];
        _generate_add_expr_code(cgh);
    
        code_list_push(cgh->cl, INS_PUSH, NULL_STRING);

        cgh->node = node->sub_list[2];
        _generate_add_expr_code(cgh);

        cgh->node = node->sub_list[1];
        _generate_rel_op_code(cgh);

        code_list_push(cgh->cl, INS_POP, NULL_STRING);
    }
}

void _generate_assign_code(code_generator_handler *cgh)
{
    int is_global = 0;
    symbol *sb = NULL;
    syntax_tree *node = cgh->node;

    // var ::= id [ '[' expr ']' ]
    code_list_push(cgh->cl, INS_PUSH, NULL_STRING);

    // is local var in current function ?
    sb = find_symbol(cgh->table, cgh->cur_space, node->sub_list[0]->data->token_str);
    
    // maybe global var ?
    if (!sb) {
        is_global = 1;
        sb = find_symbol(cgh->table, NAMESPACE_GLOBAL, node->sub_list[0]->data->token_str);
    }
    
    snprintf(cgh->size, VAR_SIZE_MAX, "%d", sb->var_idx);
    code_list_push(cgh->cl, INS_LDC, cgh->size);

    // scalar or array
    if (!node->sub_list[1]) {
        code_list_push(cgh->cl, is_global ? INS_AST : INS_ST, NULL_STRING);
    } else {
        // Get the (start) pointer (is already an absolute address)
        code_list_push(cgh->cl, is_global ? INS_ALD : INS_LD, NULL_STRING);
        code_list_push(cgh->cl, INS_PUSH, NULL_STRING);

        cgh->node = node->sub_list[0];
        _generate_expr_code(cgh);

        // Pointer[Index] (Pointer + Index)
        code_list_push(cgh->cl, INS_ADD, NULL_STRING);
        code_list_push(cgh->cl, INS_POP, NULL_STRING);

        // Save by absolute address
        code_list_push(cgh->cl, INS_AST, NULL_STRING);
    }

    code_list_push(cgh->cl, INS_POP, NULL_STRING);
}

void _generate_expr_code(code_generator_handler *cgh)
{
    syntax_tree *node = cgh->node;

    /* expr_stmt ::= [ expr ] ';'
     * expr ::= var '=' expr | simple_expr
     *
     * => expr_stmt ::= [ var '=' expr ] ';' | [ simple_expr ] ';'
     */
    if (!node->sub_list[1]) {
        cgh->node = node->sub_list[0];
        _generate_simple_expr_code(cgh);
    } else {
        cgh->node = node->sub_list[1];
        _generate_expr_code(cgh);

        cgh->node = node->sub_list[0];
        _generate_assign_code(cgh);
    }
}

void _generate_if_stmt_code(code_generator_handler *cgh)
{ 
    int cl_idx, if_size, else_size;
    syntax_tree *node = cgh->node;

    // if_stmt :: = if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]

    cgh->node = node->sub_list[0];
    _generate_expr_code(cgh);

    if (!node->sub_list[2]) {
        if_size = cgh->cl->c_idx;
        cl_idx = code_list_push(cgh->cl, INS_JZ, NULL_STRING);
        cgh->node = node->sub_list[1];

        _generate_stmt_list_code(cgh);
        if_size = cgh->cl->c_idx - if_size;
        
        snprintf(cgh->size, VAR_SIZE_MAX, "%d", if_size);
        code_list_set(cgh->cl, cl_idx, cgh->size);
    } else {

        /* if begin */
        if_size = cgh->cl->c_idx;
        cl_idx = code_list_push(cgh->cl, INS_JZ, NULL_STRING);
        cgh->node = node->sub_list[1];

        _generate_stmt_list_code(cgh);
        if_size = cgh->cl->c_idx - if_size;

        snprintf(cgh->size, VAR_SIZE_MAX, "%d", if_size + 1); // else JMP INSTRUCTION
        code_list_set(cgh->cl, cl_idx, cgh->size);
        /* if end */

        /* else begin */
        else_size = cgh->cl->c_idx;
        cl_idx = code_list_push(cgh->cl, INS_JMP, NULL_STRING);
        cgh->node = node->sub_list[2];

        _generate_stmt_list_code(cgh);
        else_size = cgh->cl->c_idx - else_size;

        snprintf(cgh->size, VAR_SIZE_MAX, "%d", else_size);
        code_list_set(cgh->cl, cl_idx, cgh->size);
        /* else end */
    }
}

void _generate_while_stmt_code(code_generator_handler *cgh)
{ 
    syntax_tree *node = cgh->node;

    // while_stmt ::= while '(' expr ')' stmt_list
    cgh->node = node->sub_list[0];
    _generate_expr_code(cgh);

    snprintf(cgh->size, VAR_SIZE_MAX, "%d", node->sub_list[1]->sub_idx + 1);
    code_list_push(cgh->cl, INS_JZ, cgh->size);

    cgh->node = node->sub_list[1];
    _generate_stmt_list_code(cgh);

    snprintf(cgh->size, VAR_SIZE_MAX, "-%d", node->sub_list[0]->sub_idx - 1);
    code_list_push(cgh->cl, INS_JMP, cgh->size);
}

void _generate_return_stmt_code(code_generator_handler *cgh)
{ 
    syntax_tree *node = cgh->node;

    // return_stmt ::= return [ expr ] ';'
    if (node->sub_list[0]) {
        cgh->node = node->sub_list[0];
        _generate_expr_code(cgh);
    }
}

void _generate_stmt_code(code_generator_handler *cgh)
{
    // stmt ::= expr_stmt | if_stmt | while_stmt | return_stmt
    
    if (NULL == cgh->node) {
        return;
    }

    switch (cgh->node->data->token_type) {
        case TOKEN_EXPR:
            _generate_expr_code(cgh);
            break;
        case TOKEN_IF_STMT:
            _generate_if_stmt_code(cgh);
            break;
        case TOKEN_WHILE_STMT:
            _generate_while_stmt_code(cgh);
            break;
        case TOKEN_RETURN_STMT:
            _generate_return_stmt_code(cgh);
            break;
        default:
            invalid_token(cgh->node->data);
    }
}

void _generate_stmt_list_code(code_generator_handler *cgh)
{
    int idx;
    syntax_tree *node;

    // stmt_list ::= { stmt }
    for (idx = 0, node = cgh->node; idx < node->sub_idx; ++idx) {
        cgh->node = node->sub_list[idx];
        _generate_stmt_code(cgh);
    }
}

void _generate_global_var_code(code_generator_handler *cgh)
{
    int s_idx, idx;
    symbol *sb;
    symbol_space *space = find_symbol_space(cgh->table, NAMESPACE_GLOBAL);

    for (s_idx = 0; s_idx < space->s_idx; ++s_idx) {
        sb = space->s[s_idx];

        // Array
        if (sb->var_size) {
            // Calc the array start address (variable number + 1)
            snprintf(cgh->size, VAR_SIZE_MAX, "%d", sb->var_idx + 1);
            code_list_push(cgh->g_cl, INS_LDC, cgh->size);
        }

        // Push the array start address
        // (Or only a meaningless int for global scalar memeory)
        code_list_push(cgh->g_cl, INS_PUSH, NULL_STRING);

        // Push array content (by array size times)
        for (idx = 0; idx < sb->var_size; ++idx) {
            code_list_push(cgh->g_cl, INS_PUSH, NULL_STRING);
        }
    }
}

void _generate_main_prepare_code(code_generator_handler *cgh)
{
    int s_idx, idx;
    symbol_space *main = find_symbol_space(cgh->table, NAMESPACE_ENTRY);

    for (s_idx = 0; s_idx < main->s_idx; ++s_idx) {
        if (main->s[s_idx]->var_size) {
            for (idx = 0; idx < main->s[s_idx]->var_size; ++idx) {
                code_list_push(cgh->g_cl, INS_PUSH, NULL_STRING);
            }
            snprintf(cgh->size, VAR_SIZE_MAX, "%d", main->s[s_idx]->var_size);
            code_list_push(cgh->g_cl, INS_ADDR, cgh->size);
            code_list_push(cgh->g_cl, INS_PUSH, NULL_STRING);
        } else {
            code_list_push(cgh->g_cl, INS_PUSH, NULL_STRING);
        }
    }

    // Call the "main" function automatically
    code_list_push(cgh->g_cl, INS_CALL, NAMESPACE_ENTRY);
}

void _set_global_code_map(code_generator_handler *cgh)
{
    _generate_global_var_code(cgh);
    _generate_main_prepare_code(cgh);

    set_code_map(cgh->c_map, NAMESPACE_GLOBAL, cgh->g_cl);
    code_list_clean(cgh->g_cl);
}

int _create_code_map(code_generator_handler *cgh)
{
    int idx;
    syntax_tree *node;

    _set_global_code_map(cgh);

    // declared_list ::= declared { declared }
    for (idx = 0; idx < cgh->tree->sub_idx; ++idx) {
        node = cgh->tree->sub_list[idx];

        // declared ::= local_declared | func_declared
        if (TOKEN_TYPE_MATCH(node, TOKEN_FUNC_DECL)) {

            // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
            cgh->cur_space = node->sub_list[1]->data->token_str;
            cgh->node = node->sub_list[4];
            _generate_stmt_list_code(cgh);

            if (strcmp(NAMESPACE_ENTRY, cgh->cur_space) != 0) {
                code_list_push(cgh->cl, INS_RET, NULL_STRING);
            }

            set_code_map(cgh->c_map, cgh->cur_space, cgh->cl);
        }

        code_list_clean(cgh->cl);
    }

    return CMM_SUCCESS;
}

int _merge_code_map(code_generator_handler *cgh)
{
    int idx, jump_num;
    char *func_name = NULL;
    map_list *maps = NULL;
    code_list *codes = NULL;

    // global code
    maps = find_map_list(cgh->c_map, NAMESPACE_GLOBAL);
    if (!maps || !maps->cl) {
        invalid_call("find_map_list global failed in merge_code_map.");
    }
    jump_num = maps->cl->c_idx;
    code_list_append(cgh->codes, maps->cl);

    // other functions
    for (idx = 0; idx < cgh->c_map->m_idx; ++idx) {
        codes = cgh->c_map->maps[idx]->cl;
        func_name = cgh->c_map->maps[idx]->name;
        if (strcmp(func_name, NAMESPACE_GLOBAL) == 0 || 
            strcmp(func_name, NAMESPACE_ENTRY) == 0) {
            continue;
        }
        code_list_append(cgh->codes, codes);
        set_func_jump_map(cgh->jumps, func_name, jump_num);
        jump_num += codes->c_idx;

    }

    // the "main" function must be the last function
    maps = find_map_list(cgh->c_map, NAMESPACE_ENTRY);
    if (!maps || !maps->cl) {
        invalid_call("find_map_list main failed in merge_code_map.");
    }
    jump_num = maps->cl->c_idx;
    code_list_append(cgh->codes, maps->cl);
    set_func_jump_map(cgh->jumps, NAMESPACE_ENTRY, jump_num);

    return CMM_SUCCESS;
}

int _translate_call(code_generator_handler *cgh)
{
    int IP, offset;

    // A virtual "IP"
    for (IP = 0; IP < cgh->codes->c_idx; ++IP) {
        if (cgh->codes->c[IP]->ins == INS_CALL) {
            offset = get_func_jump_num(cgh->jumps, cgh->codes->c[IP]->offset) - IP;
            snprintf(cgh->codes->c[IP]->offset, VAR_OFFSET_MAX, "%d", offset);
        }
    }

    return CMM_SUCCESS;
}

int generate_code(syntax_tree *ast, symbol_table *table, code_list **cl)
{
    code_generator_handler *cgh = get_code_generator_handler(ast, table);

    if (_create_code_map(cgh)) {
        invalid_call("create code map");
    }
    // code_map_print(cgh->c_map);

    if (_merge_code_map(cgh)) {
        invalid_call("merge code map");
    }

    if (_translate_call(cgh)) {
        invalid_call("translate call");
    }

    *cl = cgh->codes;

    return CMM_SUCCESS;
}
