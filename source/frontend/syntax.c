#include <stdlib.h>
#include "frontend/syntax.h"
#include "common/funcs.h"

void _expr(BridgeNode **node, syntax_tree **ast);
void _stmt(BridgeNode **node, syntax_tree **ast);
void _stmt_list(BridgeNode **node, syntax_tree **ast);

void _match_token(int type, BridgeNode **node)
{
    token *t = linked_list_node_data(*node);
    if (t->type == type) {
        *node = linked_list_node_next(*node);
    } else {
        invalid_token(t);
    }
}

void _type(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            type ::= int
                | void
    */
    syntax_tree *int_void = NULL;
    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_INT || t->type == TOKEN_VOID) {
        int_void = create_ast_node(t);
        _match_token(t->type, node);
    } else {
        invalid_token(t);
    }

    *ast = int_void;
}

void _param(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            param ::= type id [ '[' ']' ]
    */
    syntax_tree *param = NEW_AST_NODE("Param", TOKEN_PARAM);

    _type(node, &param->sub_list[param->sub_idx++]);

    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID) {
        param->sub_list[param->sub_idx++] = create_ast_node(t);
        _match_token(TOKEN_ID, node);
    } else {
        invalid_token(t);
    }

    if (t->type == TOKEN_LEFT_SQUARE_BRACKET) {
        _match_token(TOKEN_LEFT_SQUARE_BRACKET, node);
        _match_token(TOKEN_RIGHT_SQUARE_BRACKET, node);
    }

    *ast = param;
}

void _param_list(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            param_list ::= param { ',' param }
    */

    syntax_tree *param_list = NEW_AST_NODE("ParamList", TOKEN_PARAM_LIST);

    _param(node, &param_list->sub_list[param_list->sub_idx++]);

    const token *t = linked_list_node_data(*node);
    while (t->type == TOKEN_COMMA) {
        _match_token(TOKEN_COMMA, node);
        _param(node, &param_list->sub_list[param_list->sub_idx++]);
    }

    *ast = param_list;
}

void _params(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            params ::= [ param_list ]
    */

    const token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_INT || t->type == TOKEN_VOID) {
        _param_list(node, ast);
    }
}

void _var_declared(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            var_declared ::= type id [ '[' number ']' ] ';'
    */

    syntax_tree *var_decl = NEW_AST_NODE("VarDecl", TOKEN_VAR_DECL);

    _type(node, &var_decl->sub_list[var_decl->sub_idx++]);

    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID) {
        var_decl->sub_list[var_decl->sub_idx++] = create_ast_node(t);
        _match_token(TOKEN_ID, node);
    } else {
        invalid_token(t);
    }

    if (t->type == TOKEN_LEFT_SQUARE_BRACKET) {
        _match_token(TOKEN_LEFT_SQUARE_BRACKET, node);

        var_decl->sub_list[var_decl->sub_idx++] = create_ast_node(t);

        _match_token(TOKEN_NUMBER, node);
        _match_token(TOKEN_RIGHT_SQUARE_BRACKET, node);
    }

    _match_token(TOKEN_SEMICOLON, node);

    *ast = var_decl;
}

void _local_declared(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            local_declared ::= { var_declared }
    */
    syntax_tree *local_decl = NEW_AST_NODE("LocalDecl", TOKEN_LOCAL_DECL);
    const token *t = linked_list_node_data(*node);
    while (t->type == TOKEN_INT || t->type == TOKEN_VOID) {
        _var_declared(node, &local_decl->sub_list[local_decl->sub_idx++]);
    }

    *ast = local_decl;
}

void _arg_list(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            arg_list ::= expr { ',' expr }
    */

    syntax_tree *arg_list = NEW_AST_NODE("ArgList", TOKEN_ARG_LIST);

    _expr(node, &arg_list->sub_list[arg_list->sub_idx++]);

    const token *t = linked_list_node_data(*node);
    while (t->type == TOKEN_COMMA) {
        _match_token(TOKEN_COMMA, node);
        _expr(node, &arg_list->sub_list[arg_list->sub_idx++]);
    }

    *ast = arg_list;
}

void _call(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            call ::= id '(' [ arg_list ] ')'
    */

    syntax_tree *call = NEW_AST_NODE("Call", TOKEN_CALL);
    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID) {
        call->sub_list[call->sub_idx++] = create_ast_node(t);
        _match_token(TOKEN_ID, node);
    } else {
        invalid_token(t);
    }

    _match_token(TOKEN_LEFT_ROUND_BRACKET, node);

    if (t->type == TOKEN_ID ||
        t->type == TOKEN_LEFT_ROUND_BRACKET ||
        t->type == TOKEN_NUMBER) {
        _arg_list(node, &call->sub_list[call->sub_idx++]);
    }

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, node);

    *ast = call;
}

void _var(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            var ::= id [ '[' expr ']' ]
    */

    syntax_tree *var = NEW_AST_NODE("Var", TOKEN_VAR);
    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID) {
        var->sub_list[var->sub_idx++] = create_ast_node(t);
        _match_token(TOKEN_ID, node);
    } else {
        invalid_token(t);
    }

    if (t->type == TOKEN_LEFT_SQUARE_BRACKET) {
        _match_token(TOKEN_LEFT_SQUARE_BRACKET, node);
        _expr(node, &var->sub_list[var->sub_idx++]);
        _match_token(TOKEN_RIGHT_SQUARE_BRACKET, node);
    }

    *ast = var;
}

void _mul_op(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            mul_op ::= '*' | '/'
    */
    syntax_tree *op = NULL;

    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_MULTIPLY ||
        t->type == TOKEN_DIVIDE) {
        op = create_ast_node(t);
        _match_token(t->type, node);
    } else {
        invalid_token(t);
    }

    *ast = op;
}

void _factor(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            factor ::= '(' expr ')'
                     | var
                     | call
                     | num
    */
   const BridgeNode *next = NULL;
   token *t = linked_list_node_data(*node);
   switch (t->type) {
        case TOKEN_LEFT_ROUND_BRACKET:
            _match_token(TOKEN_LEFT_ROUND_BRACKET, node);
            _expr(node, ast);
            _match_token(TOKEN_RIGHT_ROUND_BRACKET, node);
            break;
        case TOKEN_NUMBER:
            *ast = create_ast_node(t);
            _match_token(TOKEN_NUMBER, node);
            break;
        case TOKEN_ID:
            next = linked_list_node_next(*node);
            t = linked_list_node_data(next);
            if (t->type == TOKEN_LEFT_ROUND_BRACKET) {
                _call(node, ast);
            } else {
                _var(node, ast);
            }
            break;
        default:
            invalid_token(t);
            break;
   }
}

void _add_op(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            add_op ::= +
                    | -
    */
    syntax_tree *op = NULL;
    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_PLUS ||
        t->type == TOKEN_MINUS) {
        op = create_ast_node(t);
        _match_token(t->type, node);
    } else {
        invalid_token(t);
    }

    *ast = op;
}

void _term(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            term ::= factor { mul_op factor }
    */
    syntax_tree *term = NEW_AST_NODE("Term", TOKEN_TERM);

    _factor(node, &term->sub_list[term->sub_idx++]);

    const token *t = linked_list_node_data(*node);
    while (t->type == TOKEN_MULTIPLY ||
           t->type == TOKEN_DIVIDE) {
        _mul_op(node, &term->sub_list[term->sub_idx++]);
        _factor(node, &term->sub_list[term->sub_idx++]);
    }

    *ast = term;
}

void _add_expr(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            add_expr ::= term { add_op term }
    */
    syntax_tree *add_expr = NEW_AST_NODE("AddExpr", TOKEN_ADD_EXPR);

    _term(node, &add_expr->sub_list[add_expr->sub_idx++]);

    const token *t = linked_list_node_data(*node);
    while (t->type == TOKEN_PLUS || 
           t->type == TOKEN_MINUS) {
        _add_op(node, &add_expr->sub_list[add_expr->sub_idx++]);
        _term(node, &add_expr->sub_list[add_expr->sub_idx++]);
    }

    *ast = add_expr;
}

void _rel_op(BridgeNode **node, syntax_tree **ast)
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
    syntax_tree *op = NULL;
    token *t = linked_list_node_data(*node);
    switch (t->type) {
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
                op = create_ast_node(t);
                _match_token(t->type, node);
                break;
        default:
                invalid_token(t);
                break;
    }

    *ast = op;
}

void _simple_expr(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            simple_expr ::= add_expr [ rel_op add_expr ]

    */
    syntax_tree *simple_expr = NEW_AST_NODE("SimpleExpr", TOKEN_SIMPLE_EXPR);
    
    _add_expr(node, &simple_expr->sub_list[simple_expr->sub_idx++]);

    const token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_LESS || t->type == TOKEN_LESS_EQUAL ||
        t->type == TOKEN_GREATER || t->type == TOKEN_GREATER_EQUAL ||
        t->type == TOKEN_EQUAL || t->type == TOKEN_NOT_EQUAL) {
        _rel_op(node, &simple_expr->sub_list[simple_expr->sub_idx++]);
        _add_expr(node, &simple_expr->sub_list[simple_expr->sub_idx++]);
    }

    *ast = simple_expr;
}

void _expr(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            expr ::= var '=' expr
                   | simple_expr
    */
    int is_assign = 0;
    syntax_tree *expr = NEW_AST_NODE("Expr", TOKEN_EXPR);

    const token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_LEFT_ROUND_BRACKET ||
        t->type == TOKEN_NUMBER) {
        _simple_expr(node, &expr->sub_list[expr->sub_idx++]);
        *ast = expr;
        return;
    } else if (t->type != TOKEN_ID) {
        invalid_token(t);
    }

    BridgeNode *next = linked_list_node_next(*node);
    t = linked_list_node_data(next);
    if (t->type == TOKEN_LEFT_ROUND_BRACKET) {
        _simple_expr(node, &expr->sub_list[expr->sub_idx++]);
    } else {
        BridgeNode *back = *node;
        _var(node, &expr->sub_list[expr->sub_idx++]);

        if (t->type == TOKEN_ASSIGN) {
            is_assign = 1;
        }

        // revert node
        free(expr->sub_list[0]);
        expr->sub_list[0] = NULL;
        expr->sub_idx = 0;
        *node = back;

        if (is_assign) {
            _var(node, &expr->sub_list[expr->sub_idx++]);
            _match_token(TOKEN_ASSIGN, node);
            _expr(node, &expr->sub_list[expr->sub_idx++]);
        } else {
            _simple_expr(node, &expr->sub_list[expr->sub_idx++]);
        }
    }

    *ast = expr;
}

void _expr_stmt(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            expr_stmt ::= [ expr ] ';'
    */

    const token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID || t->type == TOKEN_NUMBER ||
        t->type == TOKEN_LEFT_ROUND_BRACKET) { 
        _expr(node, ast);
    }

    _match_token(TOKEN_SEMICOLON, node);
}

void _if_stmt(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            if_stmt ::= if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]
    */
    token *t = NULL;
    syntax_tree *if_stmt = NEW_AST_NODE("IfStmt", TOKEN_IF_STMT);
    _match_token(TOKEN_IF, node);
    _match_token(TOKEN_LEFT_ROUND_BRACKET, node);

    _expr(node, &if_stmt->sub_list[if_stmt->sub_idx++]);

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, node);

    _match_token(TOKEN_LEFT_CURLY_BRACKET, node);

    _stmt_list(node, &if_stmt->sub_list[if_stmt->sub_idx++]);

    _match_token(TOKEN_RIGHT_CURLY_BRACKET, node);

    t = linked_list_node_data(*node);
    if (t->type == TOKEN_ELSE) {
        _match_token(TOKEN_ELSE, node);
        _match_token(TOKEN_LEFT_CURLY_BRACKET, node);

        _stmt_list(node, &if_stmt->sub_list[if_stmt->sub_idx++]);

        _match_token(TOKEN_RIGHT_CURLY_BRACKET, node);
    }

    *ast = if_stmt;
}

void _while_stmt(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            while_stmt ::= while '(' expr ')' stmt_list
    */
    syntax_tree *while_stmt = NEW_AST_NODE("WhileStmt", TOKEN_WHILE_STMT);

    _match_token(TOKEN_WHILE, node);
    _match_token(TOKEN_LEFT_ROUND_BRACKET, node);

    _expr(node, &while_stmt->sub_list[while_stmt->sub_idx++]);

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, node);
    _match_token(TOKEN_LEFT_CURLY_BRACKET, node);

    _stmt_list(node, &while_stmt->sub_list[while_stmt->sub_idx++]);

    _match_token(TOKEN_RIGHT_CURLY_BRACKET, node);

    *ast = while_stmt;
}

void _return_stmt(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            return_stmt ::= return [ expr ] ';'
    */
    syntax_tree *return_stmt = NEW_AST_NODE("ReturnStmt", TOKEN_RETURN_STMT);

    _match_token(TOKEN_RETURN, node);

    const token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID ||
        t->type == TOKEN_LEFT_ROUND_BRACKET ||
        t->type == TOKEN_NUMBER) {
        _expr(node, &return_stmt->sub_list[return_stmt->sub_idx++]);
    }

    _match_token(TOKEN_SEMICOLON, node);

    *ast = return_stmt;
}

void _stmt(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            stmt ::= expr_stmt
                   | if_stmt
                   | while_stmt
                   | return_stmt
    */

    const token *t = linked_list_node_data(*node);
    switch (t->type)
    {
        case TOKEN_SEMICOLON:
        case TOKEN_ID:
        case TOKEN_LEFT_ROUND_BRACKET:
        case TOKEN_NUMBER:
            _expr_stmt(node, ast);
            break;
        case TOKEN_IF:
            _if_stmt(node, ast);
            break;
        case TOKEN_WHILE:
            _while_stmt(node, ast);
            break;
        case TOKEN_RETURN:
            _return_stmt(node, ast);
            break;
        default:
            invalid_token(t);
            break;
   }
}

void _stmt_list(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            stmt_list ::= { stmt }
    */

    syntax_tree *stmt_list = NEW_AST_NODE("StmtList", TOKEN_STMT_LIST);
    const token *t = linked_list_node_data(*node);
    while (t->type == TOKEN_SEMICOLON || t->type== TOKEN_ID ||
           t->type== TOKEN_LEFT_ROUND_BRACKET || t->type== TOKEN_NUMBER ||
           t->type == TOKEN_IF || t->type== TOKEN_WHILE || t->type== TOKEN_RETURN) {
        _stmt(node, &stmt_list->sub_list[stmt_list->sub_idx++]);
    }

    *ast = stmt_list;
}

void _func_declared(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
    */
    syntax_tree *func_decl = NEW_AST_NODE("FuncDecl", TOKEN_FUNC_DECL);

    _type(node, &func_decl->sub_list[func_decl->sub_idx++]);

    token *t = linked_list_node_data(*node);
    if (t->type == TOKEN_ID) {
        func_decl->sub_list[func_decl->sub_idx++] = create_ast_node(t);
        _match_token(TOKEN_ID, node);
    } else {
        invalid_token(t);
    }

    _match_token(TOKEN_LEFT_ROUND_BRACKET, node);

    _params(node, &func_decl->sub_list[func_decl->sub_idx++]);

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, node);

    _match_token(TOKEN_LEFT_CURLY_BRACKET, node);

    _local_declared(node, &func_decl->sub_list[func_decl->sub_idx++]);

    _stmt_list(node, &func_decl->sub_list[func_decl->sub_idx++]);

    _match_token(TOKEN_RIGHT_CURLY_BRACKET, node);

    *ast = func_decl;
}

void _declared(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            declared ::= local_declared
                    | func_declared
    */
    const BridgeNode *next = NULL;
    const token *t = linked_list_node_data(*node);
    if (t->type != TOKEN_INT && t->type != TOKEN_VOID) {
        invalid_token(t);
    }

    next = linked_list_node_next(*node);
    t = linked_list_node_data(next);
    if (t->type != TOKEN_ID) {
        invalid_token(linked_list_node_data(next));
    }

    next = linked_list_node_next(next);
    t = linked_list_node_data(next);
    if (t->type == TOKEN_LEFT_SQUARE_BRACKET || t->type == TOKEN_SEMICOLON) {
        _var_declared(node, ast);
    } else if (t->type == TOKEN_LEFT_ROUND_BRACKET) {
        _func_declared(node, ast);
    } else {
        invalid_token(t);
    }
}

void _declared_list(BridgeNode **node, syntax_tree **ast)
{
    /*
        EBNF:
            declared_list ::= declared { declared }
    */
    syntax_tree *decl_list = NEW_AST_NODE("DeclList", TOKEN_DECL_LIST);

    while (*node) {
        _declared(node, &decl_list->sub_list[decl_list->sub_idx++]);
    }

    *ast = decl_list;
}


int syntax_analysis(compiler_handle *handle)
{
    /*
        EBNF:
            program ::= declared_list
    */
    BridgeNode *node = blist_pop_front(handle->tokens);
    _declared_list(&node, &handle->ast);
    return CMM_SUCCESS;
}
