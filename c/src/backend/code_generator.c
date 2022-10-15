#include <string.h>
#include "backend/code_generator.h"
#include "common/global_funcs.h"
void _generate_expr_code(code_generator_handler *cgh);
void _generate_stmt_list_code(code_generator_handler *cgh);

void _generate_number_code(code_generator_handler *cgh)
{
    code_list_push(cgh->cl, INS_LDC, cgh->node->data->token_str);
}

void _generate_call_code(code_generator_handler *cgh)
{
    // call ::= id '(' [ arg_list ] ')'
    syntax_tree *node = cgh->node;

    // xxx = input();
    if (strcmp(node->sub_list[0]->data->token_str, "input") == 0) {
        code_list_push(cgh->cl, INS_IN, node->data->token_str);
    }

    // output(xxx);
    else if (strcmp(node->sub_list[0]->data->token_str, "output") == 0) {
        cgh->node = node->sub_list[1]->sub_list[0];
        _generate_expr_code(cgh);

        code_list_push(cgh->cl, INS_OUT, node->data->token_str);
    }
}

void _generate_var_code(code_generator_handler *cgh)
{

}

void _generate_factor_code(code_generator_handler *cgh)
{
    // factor ::= '(' expr ')' | var | call | num
    switch (cgh->node->data->token_type) {
        case TOKEN_EXPR:
            _generate_expr_code(cgh);
        case TOKEN_NUMBER:
            _generate_number_code(cgh);
        case TOKEN_CALL:
            _generate_call_code(cgh);
        case TOKEN_VAR:
            _generate_var_code(cgh);
        default:
            invalid_token(cgh->node->data);
    }
}

void _generate_mul_op_code(code_generator_handler *cgh)
{
    // mul_op ::= '*' | '/')
    if (TOKEN_TYPE_MATCH(cgh->node, TOKEN_MULTIPLY)) {
        code_list_push(cgh->cl, INS_MUL, "");
    } else {
        code_list_push(cgh->cl, INS_DIV, "");
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
        code_list_push(cgh->cl, INS_PUSH, "");

        cgh->node = node->sub_list[idx + 1];
        _generate_factor_code(cgh);

        cgh->node = node->sub_list[idx];
        _generate_mul_op_code(cgh);

        code_list_push(cgh->cl, INS_POP, "");
    }
}

void _generate_add_op_code(code_generator_handler *cgh)
{

}

void _generate_add_expr_code(code_generator_handler *cgh)
{
    int idx;
    syntax_tree *node = cgh->node;

    // add_expr ::= term { add_op term }
    cgh->node = node->sub_list[0];
    _generate_term_code(cgh);

    for (idx = 1; idx < node->sub_idx; idx += 2) {
        code_list_push(cgh->cl, INS_PUSH, "");

        cgh->node = node->sub_list[idx + 1];
        _generate_term_code(cgh);

        cgh->node = node->sub_list[idx];
        _generate_add_op_code(cgh);

        code_list_push(cgh->cl, INS_POP, "");
    }
}

void _generate_rel_op_code(code_generator_handler *cgh)
{
    // rel_op ::= '<' | '<' '=' | '>' | '>' '=' | '=' '=' | '!' '='
    switch (cgh->node->data->token_type) {
        case TOKEN_LESS:
            code_list_push(cgh->cl, INS_LT, "");
        case TOKEN_LESS_EQUAL:
            code_list_push(cgh->cl, INS_LE, "");
        case TOKEN_GREATER:
            code_list_push(cgh->cl, INS_GT, "");
        case TOKEN_GREATER_EQUAL:
            code_list_push(cgh->cl, INS_GE, "");
        case TOKEN_EQUAL:
            code_list_push(cgh->cl, INS_EQ, "");
        case TOKEN_NOT_EQUAL:
            code_list_push(cgh->cl, INS_NE, "");
        default:
            invalid_token(cgh->node->data);
    }
}

void _generate_simple_expr_code(code_generator_handler *cgh)
{
    syntax_tree *node = cgh->node;

    // simple_expr ::= add_expr [ rel_op add_expr ]
    if (!node->sub_list[1] && !node->sub_list[2]) {
        cgh->node = node->sub_list[0];
        _generate_add_expr_code(cgh);
    } else {
        cgh->node = node->sub_list[0];
        _generate_add_expr_code(cgh);
    
        code_list_push(cgh->cl, INS_PUSH, "");

        cgh->node = node->sub_list[2];
        _generate_add_expr_code(cgh);

        cgh->node = node->sub_list[1];
        _generate_rel_op_code(cgh);

        code_list_push(cgh->cl, INS_POP, "");
    }
}

void _generate_assign_code(code_generator_handler *cgh)
{
    // // var ::= id [ '[' expr ']' ]
    // code_list_push(cgh->cl, INS_PUSH, "");

    // // if find the var in current func_name symbol space
    // if (is_local_var(NULL, func_name, node->sub_list[0]->data->token_str)) {
        
    //     snprintf(size, VAR_SIZE_MAX, "%d", get_symbol_idx(symbol));
    //     code_list_push(cgh->cl, INS_LDC, size);
    // } else {

    // }

    // code_list_push(cgh->cl, INS_POP, "");
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
        cgh->node = node->sub_list[1];
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
    syntax_tree *node = cgh->node;

    // if_stmt :: = if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]

    cgh->node = node->sub_list[0];
    _generate_expr_code(cgh);

    if (!node->sub_list[2]) {
        snprintf(cgh->size, VAR_SIZE_MAX, "%d", node->sub_list[1]->sub_idx);
        code_list_push(cgh->cl, INS_JZ, cgh->size);

        cgh->node = node->sub_list[1];
        _generate_stmt_list_code(cgh);
    } else {
        snprintf(cgh->size, VAR_SIZE_MAX, "%d", node->sub_list[1]->sub_idx);
        code_list_push(cgh->cl, INS_JZ, cgh->size);

        cgh->node = node->sub_list[1];
        _generate_stmt_list_code(cgh);

        snprintf(cgh->size, VAR_SIZE_MAX, "%d", node->sub_list[2]->sub_idx);
        code_list_push(cgh->cl, INS_JMP, cgh->size);

        cgh->node = node->sub_list[2];
        _generate_stmt_list_code(cgh);
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
    switch (cgh->node->data->token_type) {
        case TOKEN_EXPR:
            _generate_expr_code(cgh);
        case TOKEN_IF_STMT:
            _generate_if_stmt_code(cgh);
        case TOKEN_WHILE_STMT:
            _generate_while_stmt_code(cgh);
        case TOKEN_RETURN_STMT:
            _generate_return_stmt_code(cgh);
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

void _set_global_code_map(code_generator_handler *cgh)
{

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

            if (strcmp("main", cgh->cur_space) != 0) {
                code_list_push(cgh->cl, INS_RET, "");
            }

            set_code_map(cgh->c_map, cgh->cur_space, cgh->cl);
        }
    }

    return CMM_SUCCESS;
}

int _merge_code_map(code_map *c_map)
{
    return CMM_SUCCESS;
}

int _translate_call(code_map *c_map)
{
    return CMM_SUCCESS;
}

int generate_code(syntax_tree *ast, symbol_table *table)
{  
    code_generator_handler *cgh = get_code_generator_handler(ast, table);

    if (_create_code_map(cgh)) {
        invalid_call("create code map");
    }

    if (_merge_code_map(cgh->c_map)) {
        invalid_call("merge code map");
    }

    if (_translate_call(cgh->c_map)) {
        invalid_call("translate call");
    }

    return CMM_SUCCESS;
}