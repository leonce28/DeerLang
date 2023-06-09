#include <stdlib.h>
#include "frontend/syntax.h"
#include "common/funcs.h"

static DeerListCell *cell = nullptr;

DeerMultiTree *mount_expr();
DeerMultiTree *mount_stmt();
DeerMultiTree *mount_stmt_list();

void match_token(TokenType type)
{
    DeerToken *token = dcell_data(DeerToken, cell);
    if (token->type == type) {
        cell = dcell_next(cell);
    } else {
        invalid_token(token);
    }
}

DeerMultiTree *mount_type()
{
    /*
        EBNF:
            type ::= int
                | void
    */
    DeerMultiTree *int_void = nullptr;
    if (TOKEN_IS_MATCH(TOKEN_INT) || TOKEN_IS_MATCH(TOKEN_VOID)) {
        int_void = dtree_create(CURRENT_TOKEN());
        match_token(CURRENT_TOKEN_TYPE());
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return int_void;
}

DeerMultiTree *mount_param()
{
    /*
        EBNF:
            param ::= type id [ '[' ']' ]
    */
    DeerMultiTree *param = NEW_AST_NODE("Param", TOKEN_PARAM);
    dtree_append(param, mount_type());

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        dtree_append(param, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    if (TOKEN_IS_MATCH(TOKEN_LEFT_SQUARE_BRACKET)) {
        match_token(TOKEN_LEFT_SQUARE_BRACKET);
        match_token(TOKEN_RIGHT_SQUARE_BRACKET);
    }

    return param;
}

DeerMultiTree *mount_param_list()
{
    /*
        EBNF:
            param_list ::= param { ',' param }
    */
    DeerMultiTree *param_list = NEW_AST_NODE("ParamList", TOKEN_PARAM_LIST);

    dtree_append(param_list, mount_param());

    while (TOKEN_IS_MATCH(TOKEN_COMMA)) {
        match_token(TOKEN_COMMA);
        dtree_append(param_list, mount_param());
    }

    return param_list;
}

DeerMultiTree *mount_params()
{
    /*
        EBNF:
            params ::= [ param_list ]
    */

    if (TOKEN_IS_MATCH(TOKEN_INT) || 
        TOKEN_IS_MATCH(TOKEN_VOID)) {
        return mount_param_list();
    }

    return nullptr;
}

DeerMultiTree *mount_var_declared()
{
    /*
        EBNF:
            var_declared ::= type id [ '[' number ']' ] ';'
    */

    DeerMultiTree *var_decl = NEW_AST_NODE("VarDecl", TOKEN_VAR_DECL);

    dtree_append(var_decl, mount_type());

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        dtree_append(var_decl, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    if (TOKEN_IS_MATCH(TOKEN_LEFT_SQUARE_BRACKET)) {
        match_token(TOKEN_LEFT_SQUARE_BRACKET);
        dtree_append(var_decl, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_NUMBER);
        match_token(TOKEN_RIGHT_SQUARE_BRACKET);
    }

    match_token(TOKEN_SEMICOLON);

    return var_decl;
}

DeerMultiTree *mount_local_declared()
{
    /*
        EBNF:
            local_declared ::= { var_declared }
    */
    DeerMultiTree *local_decl = NEW_AST_NODE("LocalDecl", TOKEN_LOCAL_DECL);

    while (TOKEN_IS_MATCH(TOKEN_INT) || 
           TOKEN_IS_MATCH(TOKEN_VOID)) {
        dtree_append(local_decl, mount_var_declared());
    }

    return local_decl;
}

DeerMultiTree *mount_arg_list()
{
    /*
        EBNF:
            arg_list ::= expr { ',' expr }
    */

    DeerMultiTree *arg_list = NEW_AST_NODE("ArgList", TOKEN_ARG_LIST);

    dtree_append(arg_list, mount_expr());

    while (TOKEN_IS_MATCH(TOKEN_COMMA)) {
        match_token(TOKEN_COMMA);

        dtree_append(arg_list, mount_expr());
    }

    return arg_list;
}

DeerMultiTree *mount_call()
{
    /*
        EBNF:
            call ::= id '(' [ arg_list ] ')'
    */

    DeerMultiTree *call = NEW_AST_NODE("Call", TOKEN_CALL);

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        dtree_append(call, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    match_token(TOKEN_LEFT_ROUND_BRACKET);

    if (TOKEN_IS_MATCH(TOKEN_ID) || TOKEN_IS_MATCH(TOKEN_NUMBER) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET)) {
        dtree_append(call, mount_arg_list());
    }

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    return call;
}

DeerMultiTree *mount_var()
{
    /*
        EBNF:
            var ::= id [ '[' expr ']' ]
    */

    DeerMultiTree *var = NEW_AST_NODE("Var", TOKEN_VAR);
    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        dtree_append(var, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    if (TOKEN_IS_MATCH(TOKEN_LEFT_SQUARE_BRACKET)) {
        match_token(TOKEN_LEFT_SQUARE_BRACKET);
        dtree_append(var, mount_expr());
        match_token(TOKEN_RIGHT_SQUARE_BRACKET);
    }

    return var;
}

DeerMultiTree *mount_mul_op()
{
    /*
        EBNF:
            mul_op ::= '*' 
                    | '/'
    */
    DeerMultiTree *op = nullptr;
    if (TOKEN_IS_MATCH(TOKEN_MULTIPLY) ||
        TOKEN_IS_MATCH(TOKEN_DIVIDE)) {
        op = dtree_create(CURRENT_TOKEN());
        match_token(CURRENT_TOKEN_TYPE());
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return op;
}

DeerMultiTree *mount_factor()
{
    /*
        EBNF:
            factor ::= '(' expr ')'
                     | var
                     | call
                     | num
    */
   DeerMultiTree *factor = nullptr;
   const DeerListCell *next = nullptr;
   switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LEFT_ROUND_BRACKET:
            match_token(TOKEN_LEFT_ROUND_BRACKET);
            factor = mount_expr();
            match_token(TOKEN_RIGHT_ROUND_BRACKET);
            break;
        case TOKEN_NUMBER:
            factor = dtree_create(CURRENT_TOKEN());
            match_token(TOKEN_NUMBER);
            break;
        case TOKEN_ID:
            next = dcell_next(cell);
            if (next && TOKEN_IS_MATCH2(dcell_data(DeerToken, next), 
                                        TOKEN_LEFT_ROUND_BRACKET)) {
                factor = mount_call();
            } else {
                factor = mount_var();
            }
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
   }

   return factor;
}

DeerMultiTree *mount_add_op()
{
    /*
        EBNF:
            add_op ::= +
                    | -
    */
    DeerMultiTree *op = nullptr;
    if (TOKEN_IS_MATCH(TOKEN_PLUS) ||
        TOKEN_IS_MATCH(TOKEN_MINUS)) {
        op = dtree_create(CURRENT_TOKEN());
        match_token(CURRENT_TOKEN_TYPE());
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return op;
}

DeerMultiTree *mount_term()
{
    /*
        EBNF:
            term ::= factor { mul_op factor }
    */
    DeerMultiTree *term = NEW_AST_NODE("Term", TOKEN_TERM);

    dtree_append(term, mount_factor());

    while (TOKEN_IS_MATCH(TOKEN_MULTIPLY) || 
           TOKEN_IS_MATCH(TOKEN_DIVIDE)) {
        dtree_append(term, mount_mul_op());
        dtree_append(term, mount_factor());
    }

    return term;
}

DeerMultiTree *mount_add_expr()
{
    /*
        EBNF:
            add_expr ::= term { add_op term }
    */
    DeerMultiTree *add_expr = NEW_AST_NODE("AddExpr", TOKEN_ADD_EXPR);

    dtree_append(add_expr, mount_term());

    while (TOKEN_IS_MATCH(TOKEN_PLUS) || 
           TOKEN_IS_MATCH(TOKEN_MINUS)) {
        dtree_append(add_expr, mount_add_op());
        dtree_append(add_expr, mount_term());
    }

    return add_expr;
}

DeerMultiTree *mount_rel_op()
{ 
    /*
        EBNF:
            rel_op ::= <
                    | <=
                    | >
                    | >=
                    | ==
                    | !=
    */
    DeerMultiTree *op = nullptr;
    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
                op = dtree_create(CURRENT_TOKEN());
                match_token(CURRENT_TOKEN_TYPE());
                break;
        default:
                invalid_token(CURRENT_TOKEN());
                break;
    }

    return op;
}

DeerMultiTree *mount_simple_expr()
{
    /*
        EBNF:
            simple_expr ::= add_expr [ rel_op add_expr ]

    */
    DeerMultiTree *simple_expr = NEW_AST_NODE("SimpleExpr", TOKEN_SIMPLE_EXPR);
    
    dtree_append(simple_expr, mount_add_expr());

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
            dtree_append(simple_expr, mount_rel_op());
            dtree_append(simple_expr, mount_add_expr());
            break;
        default:
            break;
    }

    return simple_expr;
}

DeerMultiTree *mount_expr()
{
    /*
        EBNF:
            expr ::= var '=' expr
                   | simple_expr
    */
    DeerMultiTree *expr = NEW_AST_NODE("Expr", TOKEN_EXPR);

    if (TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET) ||
        TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        dtree_append(expr, mount_simple_expr());
        return expr;
    } else if (!TOKEN_IS_MATCH(TOKEN_ID)) {
        invalid_token(CURRENT_TOKEN());
    }

    DeerListCell *next = dcell_next(cell);
    if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_ROUND_BRACKET)) {
        dtree_append(expr, mount_simple_expr());
    } else {
        DeerListCell *back = cell;
        dtree_append(expr, mount_var());

        if (TOKEN_IS_MATCH(TOKEN_ASSIGN)) {
            match_token(TOKEN_ASSIGN);
            dtree_append(expr, mount_expr());
        } else {
            // revert cell, clean all childs, todo
            dtree_clean_clilds(expr);
            expr = nullptr;
            cell = back;
            dtree_append(expr, mount_simple_expr());
        }
    }

    return expr;
}

DeerMultiTree *mount_expr_stmt()
{
    /*
        EBNF:
            expr_stmt ::= [ expr ] ';'
    */
    DeerMultiTree *expr_stmt = nullptr;

    if (TOKEN_IS_MATCH(TOKEN_ID) || TOKEN_IS_MATCH(TOKEN_NUMBER) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET)) { 
        expr_stmt = mount_expr();
    }

    match_token(TOKEN_SEMICOLON);
    return expr_stmt;
}

DeerMultiTree *mount_if_stmt()
{
    /*
        EBNF:
            if_stmt ::= if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]
    */
    DeerMultiTree *if_stmt = NEW_AST_NODE("IfStmt", TOKEN_IF_STMT);
    match_token(TOKEN_IF);
    match_token(TOKEN_LEFT_ROUND_BRACKET);

    dtree_append(if_stmt, mount_expr());

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    match_token(TOKEN_LEFT_CURLY_BRACKET);

    dtree_append(if_stmt, mount_stmt_list());

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    if (TOKEN_IS_MATCH(TOKEN_ELSE)) {
        match_token(TOKEN_ELSE);
        match_token(TOKEN_LEFT_CURLY_BRACKET);

        dtree_append(if_stmt, mount_stmt_list());

        match_token(TOKEN_RIGHT_CURLY_BRACKET);
    }

    return if_stmt;
}

DeerMultiTree *mount_while_stmt()
{
    /*
        EBNF:
            while_stmt ::= while '(' expr ')' stmt_list
    */
    DeerMultiTree *while_stmt = NEW_AST_NODE("WhileStmt", TOKEN_WHILE_STMT);

    match_token(TOKEN_WHILE);
    match_token(TOKEN_LEFT_ROUND_BRACKET);

    dtree_append(while_stmt, mount_expr());

    match_token(TOKEN_RIGHT_ROUND_BRACKET);
    match_token(TOKEN_LEFT_CURLY_BRACKET);

    dtree_append(while_stmt, mount_stmt_list());

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    return while_stmt;
}

DeerMultiTree *mount_return_stmt()
{
    /*
        EBNF:
            return_stmt ::= return [ expr ] ';'
    */
    DeerMultiTree *return_stmt = NEW_AST_NODE("ReturnStmt", TOKEN_RETURN_STMT);

    match_token(TOKEN_RETURN);

    if (TOKEN_IS_MATCH(TOKEN_ID) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET) ||
        TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        dtree_append(return_stmt, mount_expr());
    }

    match_token(TOKEN_SEMICOLON);

    return return_stmt;
}

DeerMultiTree *mount_stmt()
{
    /*
        EBNF:
            stmt ::= expr_stmt
                   | if_stmt
                   | while_stmt
                   | return_stmt
    */

    DeerMultiTree *stmt = nullptr;
    const DeerToken *token = dcell_data(DeerToken, cell);
    switch (token->type) {
        case TOKEN_SEMICOLON:
        case TOKEN_ID:
        case TOKEN_LEFT_ROUND_BRACKET:
        case TOKEN_NUMBER:
            stmt = mount_expr_stmt();
            break;
        case TOKEN_IF:
            stmt = mount_if_stmt();
            break;
        case TOKEN_WHILE:
            stmt = mount_while_stmt();
            break;
        case TOKEN_RETURN:
            stmt = mount_return_stmt();
            break;
        default:
            invalid_token(token);
            break;
   }

   return stmt;
}

DeerMultiTree *mount_stmt_list()
{
    /*
        EBNF:
            stmt_list ::= { stmt }
    */

    
    DeerMultiTree *stmt_list = NEW_AST_NODE("StmtList", TOKEN_STMT_LIST);

    for (bool flag = true; flag && cell; ) {
        switch (CURRENT_TOKEN_TYPE()) {
            case TOKEN_SEMICOLON:
            case TOKEN_ID:
            case TOKEN_LEFT_ROUND_BRACKET:
            case TOKEN_NUMBER:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
                dtree_append(stmt_list, mount_stmt());
                break;
            default:
                flag = false;
                break;
        }
    }

    return stmt_list;
}

DeerMultiTree *mount_func_declared()
{
    /*
        EBNF:
            func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
    */
    DeerMultiTree *func_decl = NEW_AST_NODE("FuncDecl", TOKEN_FUNC_DECL);

    dtree_append(func_decl, mount_type());

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        dtree_append(func_decl, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    match_token(TOKEN_LEFT_ROUND_BRACKET);

    dtree_append(func_decl, mount_params());

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    match_token(TOKEN_LEFT_CURLY_BRACKET);

    dtree_append(func_decl, mount_local_declared());

    dtree_append(func_decl, mount_stmt_list());

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    return func_decl;
}

DeerMultiTree *mount_declared()
{
    /*
        EBNF:
            declared ::= local_declared
                        | func_declared
    */
    DeerMultiTree *declared = nullptr;
    const DeerListCell *next = nullptr;
    if (!TOKEN_IS_MATCH(TOKEN_INT) && !TOKEN_IS_MATCH(TOKEN_VOID)) {
        invalid_token(CURRENT_TOKEN());
    }

    next = dcell_next(cell);
    if (!TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_ID)) {
        invalid_token(dcell_data(DeerToken, next));
    }

    next = dcell_next(next);
    if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_SQUARE_BRACKET) || 
        TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_SEMICOLON)) {
        declared = mount_var_declared();
    } else if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_ROUND_BRACKET)) {
        declared = mount_func_declared();
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return declared;
}

DeerMultiTree *mount_declared_list()
{
    /*
        EBNF:
            declared_list ::= declared { declared }
    */
    DeerMultiTree *decl_list = NEW_AST_NODE("DeclList", TOKEN_DECL_LIST);

    while (cell) {
        dtree_append(decl_list, mount_declared());
    }

    return decl_list;
}


int syntax_analysis(DeerCompilerHandle *handle)
{
    /*
        EBNF:
            program ::= declared_list
    */
    cell = dlist_head(handle->tokens);

    handle->ast = mount_declared_list();

    return CMM_SUCCESS;
}
