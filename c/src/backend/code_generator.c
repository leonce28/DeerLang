#include <assert.h>
#include "backend/code_generator.h"
#include "common/global_funcs.h"

void _generate_number_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    code_list_push(cl, INS_LDC, node->data->token_str);
}

void _generate_call_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    // call ::= id '(' [ arg_list ] ')'

    // xxx = input();
    if (strcmp(node->sub_list[0]->data->token_str, "input") == 0) {
        code_list_push(cl, INS_IN, node->data->token_str);
    }

    // output(xxx);
    else if (strcmp(node->sub_list[0]->data->token_str, "output") == 0) {
        _generate_expr_code(node->sub_list[1]->sub_list[0], func_name, cl);
        code_list_push(cl, INS_OUT, node->data->token_str);
    }
}

void _generate_var_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

}

void _generate_factor_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    // factor ::= '(' expr ')' | var | call | num
    switch (node->data->token_type) {
        case TOKEN_EXPR:
            _generate_expr_code(node, func_name, cl);
        case TOKEN_NUMBER:
            _generate_number_code(node, func_name, cl);
        case TOKEN_CALL:
            _generate_call_code(node, func_name, cl);
        case TOKEN_VAR:
            _generate_var_code(node, func_name, cl);
        default:
            invalid_token(node->data);
    }
}

void _generate_mul_op_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);
}

void _generate_term_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    int idx;
    // term ::= factor { mul_op factor }
    _generate_factor_code(node->sub_list[0], func_name, cl);

    for (idx = 1; idx < node->sub_idx; idx += 2) {
        core_list_push(cl, INS_PUSH, "");
        _generate_factor_code(node->sub_list[idx + 1], func_name, cl);
        _generate_mul_op_code(node->sub_list[idx], func_name, cl);
        core_list_push(cl, INS_POP, "");
    }
}

void _generate_add_op_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);
}

void _generate_add_expr_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    int idx;
    // add_expr ::= term { add_op term }
    _generate_term_code(node->sub_list[0], func_name, cl);

    for (idx = 1; idx < node->sub_idx; idx += 2) {
        core_list_push(cl, INS_PUSH, "");
        _generate_term_code(node->sub_list[idx + 1], func_name, cl);
        _generate_add_op_code(node->sub_list[idx], func_name, cl);
        core_list_push(cl, INS_POP, "");
    }
}

void _generate_rel_op_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

}

void _generate_simple_expr_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    // simple_expr ::= add_expr [ rel_op add_expr ]
    if (!node->sub_list[1] && !node->sub_list[2]) {
        _generate_add_expr_code(node->sub_list[0], func_name, cl);
    } else {
        _generate_add_expr_code(node->sub_list[0], func_name, cl);
        code_list_push(cl, INS_PUSH, "");
        _generate_add_expr_code(node->sub_list[2], func_name, cl);
        _generate_rel_op_code(node->sub_list[1], func_name, cl);
        code_list_push(cl, INS_POP, "");
    }

}

void _generate_assign_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    // var ::= id [ '[' expr ']' ]
    code_list_push(cl, INS_PUSH, "");

        // Local variable
    if (__symbolTable.at(curFuncName).count(root->__subList[0]->__tokenStr)) {
        codeList.emplace_back(__Instruction::__LDC, to_string(__symbolTable.at(curFuncName).at(root->__subList[0]->__tokenStr).first));

        // Scalar
        if (!root->__subList[1]) {
            codeList.emplace_back(__Instruction::__ST, "");
        }
        // Array
        else {
            auto exprCodeList = __generateExprCode(root->__subList[1], curFuncName);

            // Get the (start) pointer (is already an absolute address)
            codeList.emplace_back(__Instruction::__LD, "");
            codeList.emplace_back(__Instruction::__PUSH, "");

            codeList.insert(codeList.end(), exprCodeList.begin(), exprCodeList.end());

            // Pointer[Index] (Pointer + Index)
            codeList.emplace_back(__Instruction::__ADD, "");
            codeList.emplace_back(__Instruction::__POP, "");

            // Save by absolute address
            codeList.emplace_back(__Instruction::__AST, "");
        }
    }
    // Global variable
    else {
        codeList.emplace_back(__Instruction::__LDC, to_string(__symbolTable.at("__GLOBAL__").at(root->__subList[0]->__tokenStr).first));

        // Scalar
        if (!root->__subList[1]) {
            codeList.emplace_back(__Instruction::__AST, "");
        }
        // Array
        else {
            auto exprCodeList = __generateExprCode(root->__subList[1], curFuncName);

            // Absolute get the (start) pointer (is already an absolute address)
            codeList.emplace_back(__Instruction::__ALD, "");
            codeList.emplace_back(__Instruction::__PUSH, "");

            codeList.insert(codeList.end(), exprCodeList.begin(), exprCodeList.end());

            // Pointer[Index] (Pointer + Index)
            codeList.emplace_back(__Instruction::__ADD, "");
            codeList.emplace_back(__Instruction::__POP, "");

            // Save by absolute address
            codeList.emplace_back(__Instruction::__AST, "");
        }
    }

    codeList.emplace_back(__Instruction::__POP, "");
}

void _generate_expr_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    /* expr_stmt ::= [ expr ] ';'
     * expr ::= var '=' expr | simple_expr
     *
     * => expr_stmt ::= [ var '=' expr ] ';' | [ simple_expr ] ';'
     */
    if (!node->sub_list[1]) {
        _generate_simple_expr_code(node->sub_list[1], func_name, cl);
    } else {
        _generate_expr_code(node->sub_list[1], func_name, cl);
        _generate_assign_code(node->sub_list[0], func_name, cl);
    }
}

void _generate_if_stmt_code(const syntax_tree *node, char *func_name, code_list *cl)
{ 
    assert(node != NULL && func_name != NULL && cl != NULL);

}

void _generate_while_stmt_code(const syntax_tree *node, char *func_name, code_list *cl)
{ 
    assert(node != NULL && func_name != NULL && cl != NULL);

}

void _generate_return_stmt_code(const syntax_tree *node, char *func_name, code_list *cl)
{ 
    assert(node != NULL && func_name != NULL && cl != NULL);

}

void _generate_stmt_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    // stmt ::= expr_stmt | if_stmt | while_stmt | return_stmt
    switch (node->data->token_type) {
        case TOKEN_EXPR:
            _generate_expr_code(node, func_name, cl);
        case TOKEN_IF_STMT:
            _generate_if_stmt_code(node, func_name, cl);
        case TOKEN_WHILE_STMT:
            _generate_while_stmt_code(node, func_name, cl);
        case TOKEN_RETURN_STMT:
            _generate_return_stmt_code(node, func_name, cl);
        default:
            invalid_token(node->data);
    }
}

void _generate_stmt_list_code(const syntax_tree *node, char *func_name, code_list *cl)
{
    assert(node != NULL && func_name != NULL && cl != NULL);

    int idx;

    // stmt_list ::= { stmt }
    for (idx = 0; idx < node->sub_idx; ++idx) { 
        _generate_stmt_code(node->sub_list[idx], func_name, cl);
    }
}

int _create_unordered_code_map(const syntax_tree *ast, const symbol_table *table, unordered_code_map **ucm)
{
    assert(ast != NULL && table != NULL);

    int idx;
    code_map *map = NULL;
    const char* cur_func_name;
    const syntax_tree *node;
    code_list *cl = create_code_list();
    unordered_code_map *u = create_unordered_code_map();

    // declared_list ::= declared { declared }
    for (idx = 0; idx < ast->sub_idx; ++idx) {
        node = ast->sub_list[idx];

        // declared ::= local_declared | func_declared
        if (node->data->token_type == TOKEN_FUNC_DECL) {
            
            // func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
            cur_func_name = node->sub_list[1]->data->token_str;

            _generate_stmt_list_code(node->sub_list[4], cur_func_name, cl);

            if (strcmp("main", cur_func_name) != 0) {
                cl_push_back(cl, INS_RET, "");
            }

            set_code_map(u, cur_func_name, cl);
        }
    }

    *ucm = u;
    return CMM_SUCCESS;
}

int _merge_code_map(unordered_code_map **ucm)
{
    return CMM_SUCCESS;
}

int _translate_call(unordered_code_map **ucm)
{
    return CMM_SUCCESS;
}

int generate_code(const syntax_tree *ast, const symbol_table *table, unordered_code_map **ucm)
{  
    if (_create_unordered_code_map(ast, table, ucm)) {
        invalid_call("create code map");
    }

    if (_merge_code_map(ucm)) {
        invalid_call("merge code map");
    }

    if (_translate_call(ucm)) {
        invalid_call("translate call");
    }

    return CMM_SUCCESS;
}