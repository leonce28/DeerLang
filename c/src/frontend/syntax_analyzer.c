#include <stdlib.h>
#include "frontend/syntax_analyzer.h"
#include "common/global_funcs.h"

void _expr(token_list *tokens, int *token_idx, syntax_tree **ast);
void _stmt(token_list *tokens, int *token_idx, syntax_tree **ast);
void _stmt_list(token_list *tokens, int *token_idx, syntax_tree **ast);

void _match_token(int type, token_list *tokens, int *token_idx)
{
    if (ANALY_TOKEN_TYPE() == type) {
        (*token_idx)++;
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }
}

void _type(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            type ::= int
                | void
    */
    syntax_tree *node = NULL;
    if (ANALY_TOKEN_TYPE() == TOKEN_INT ||
        ANALY_TOKEN_TYPE() == TOKEN_VOID) {
        node = NEW_AST_NODE2(ANALY_TOKEN_PTR());
        _match_token(ANALY_TOKEN_TYPE(), tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    *ast = node;
}

void _param(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            param ::= type id [ '[' ']' ]
    */
    syntax_tree *node = NEW_AST_NODE("Param", TOKEN_PARAM);

    _type(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID) {
        node->sub_list[node->sub_idx++] = NEW_AST_NODE3();
        _match_token(TOKEN_ID, tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    if (ANALY_TOKEN_TYPE() == TOKEN_LEFT_SQUARE_BRACKET) {
        _match_token(TOKEN_LEFT_SQUARE_BRACKET, tokens, token_idx);
        _match_token(TOKEN_RIGHT_SQUARE_BRACKET, tokens, token_idx);
    }

    *ast = node;
}

void _param_list(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            param_list ::= param { ',' param }
    */

    syntax_tree *node = NEW_AST_NODE("ParamList", TOKEN_PARAM_LIST);

    _param(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    while (ANALY_TOKEN_TYPE() == TOKEN_COMMA) {
        _match_token(TOKEN_COMMA, tokens, token_idx);
        _param(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _params(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            params ::= [ param_list ]
    */

    if (ANALY_TOKEN_TYPE() == TOKEN_INT ||
        ANALY_TOKEN_TYPE() == TOKEN_VOID) {
        _param_list(tokens, token_idx, ast);
    }
}

void _var_decl(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            var_decl ::= type id [ '[' number ']' ] ';'
    */

    syntax_tree *node = NEW_AST_NODE("VarDecl", TOKEN_VAR_DECL);

    _type(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID) {
        node->sub_list[node->sub_idx++] = NEW_AST_NODE3();
        _match_token(TOKEN_ID, tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    if (ANALY_TOKEN_TYPE() == TOKEN_LEFT_SQUARE_BRACKET) {
        _match_token(TOKEN_LEFT_SQUARE_BRACKET, tokens, token_idx);

        node->sub_list[node->sub_idx++] = NEW_AST_NODE3();

        _match_token(TOKEN_NUMBER, tokens, token_idx);
        _match_token(TOKEN_RIGHT_SQUARE_BRACKET, tokens, token_idx);
    }

    _match_token(TOKEN_SEMICOLON, tokens, token_idx);

    *ast = node;
}

void _local_declared(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            local_declared ::= { var_decl }
    */
    syntax_tree *node = NEW_AST_NODE("LocalDecl", TOKEN_LOCAL_DECL);

    while (ANALY_TOKEN_TYPE() == TOKEN_INT || 
           ANALY_TOKEN_TYPE() == TOKEN_VOID) {
        _var_decl(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _arg_list(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            arg_list ::= expr { ',' expr }
    */

    syntax_tree *node = NEW_AST_NODE("ArgList", TOKEN_ARG_LIST);

    _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    while (ANALY_TOKEN_TYPE() == TOKEN_COMMA) {
        _match_token(TOKEN_COMMA, tokens, token_idx);
        _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _call(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            call ::= id '(' [ arg_list ] ')'
    */

    syntax_tree *node = NEW_AST_NODE("Call", TOKEN_CALL);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID) {
        node->sub_list[node->sub_idx++] = NEW_AST_NODE3();
        _match_token(TOKEN_ID, tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    _match_token(TOKEN_LEFT_ROUND_BRACKET, tokens, token_idx);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID ||
        ANALY_TOKEN_TYPE() == TOKEN_LEFT_ROUND_BRACKET ||
        ANALY_TOKEN_TYPE() == TOKEN_NUMBER) {
        _arg_list(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, tokens, token_idx);
    
    *ast = node;
}

void _var(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            var ::= id [ '[' expr ']' ]
    */

    syntax_tree *node = NEW_AST_NODE("Var", TOKEN_VAR);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID) {
        node->sub_list[node->sub_idx++] = NEW_AST_NODE3();
        _match_token(TOKEN_ID, tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    if (ANALY_TOKEN_TYPE() == TOKEN_LEFT_SQUARE_BRACKET) {
        _match_token(TOKEN_LEFT_SQUARE_BRACKET, tokens, token_idx);
        _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        _match_token(TOKEN_RIGHT_SQUARE_BRACKET, tokens, token_idx);
    }

    *ast = node;
}

void _mul_op(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            mul_op ::= '*' | '/'
    */
    syntax_tree *node = NULL;

    if (ANALY_TOKEN_TYPE() == TOKEN_MULTIPLY ||
        ANALY_TOKEN_TYPE() == TOKEN_DIVIDE) {
        node = NEW_AST_NODE3();
        _match_token(ANALY_TOKEN_TYPE(), tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    *ast = node;
}

void _factor(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            factor ::= '(' expr ')'
                     | var
                     | call
                     | num
    */
   switch (ANALY_TOKEN_TYPE()) {
        case TOKEN_LEFT_ROUND_BRACKET:
            _match_token(TOKEN_LEFT_ROUND_BRACKET, tokens, token_idx);
            _expr(tokens, token_idx, ast);
            _match_token(TOKEN_RIGHT_ROUND_BRACKET, tokens, token_idx);
            break;
        case TOKEN_NUMBER:
            *ast = NEW_AST_NODE3();
            _match_token(TOKEN_NUMBER, tokens, token_idx);
            break;
        case TOKEN_ID:
            if (ANALY_TOKEN_TYPE2(1) == TOKEN_LEFT_ROUND_BRACKET) {
                _call(tokens, token_idx, ast);
            } else {
                _var(tokens, token_idx, ast);
            }
            break;
        default:
            invalid_token(ANALY_TOKEN_PTR());
            break;
   }
}

void _add_op(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            add_op ::= +
                    | -
    */
   syntax_tree *node = NULL;
    if (ANALY_TOKEN_TYPE() == TOKEN_PLUS ||
        ANALY_TOKEN_TYPE() == TOKEN_MINUS) {
        node = NEW_AST_NODE3();
        _match_token(ANALY_TOKEN_TYPE(), tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    *ast = node;
}

void _term(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            term ::= factor { mul_op factor }
    */
    syntax_tree *node = NEW_AST_NODE("Term", TOKEN_TERM);

    _factor(tokens, token_idx, &node->sub_list[0]);

    while (ANALY_TOKEN_TYPE() == TOKEN_MULTIPLY ||
           ANALY_TOKEN_TYPE() == TOKEN_DIVIDE) {
        _mul_op(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        _factor(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _add_expr(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            add_expr ::= term { add_op term }
    */
    syntax_tree *node = NEW_AST_NODE("AddExpr", TOKEN_ADD_EXPR);

    _term(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    while (ANALY_TOKEN_TYPE() == TOKEN_PLUS || 
           ANALY_TOKEN_TYPE() == TOKEN_MINUS) {
        _add_op(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        _term(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _rel_op(token_list *tokens, int *token_idx, syntax_tree **ast)
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

   syntax_tree *node = NULL;

   switch (ANALY_TOKEN_TYPE()) {
       case TOKEN_LESS:
       case TOKEN_LESS_EQUAL:
       case TOKEN_GREATER:
       case TOKEN_GREATER_EQUAL:
       case TOKEN_EQUAL:
       case TOKEN_NOT_EQUAL:
            node = NEW_AST_NODE3();
            _match_token(ANALY_TOKEN_TYPE(), tokens, token_idx);
            break;
       default:
            invalid_token(ANALY_TOKEN_PTR());
            break;
   }

    *ast = node;
}

void _simple_expr(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            simple_expr ::= add_expr [ rel_op add_expr ]

    */
    syntax_tree *node = NEW_AST_NODE("SimpleExpr", TOKEN_SIMPLE_EXPR);

    _add_expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    if (ANALY_TOKEN_TYPE() == TOKEN_LESS || 
        ANALY_TOKEN_TYPE() == TOKEN_LESS_EQUAL ||
        ANALY_TOKEN_TYPE() == TOKEN_GREATER ||
        ANALY_TOKEN_TYPE() == TOKEN_GREATER_EQUAL ||
        ANALY_TOKEN_TYPE() == TOKEN_EQUAL ||
        ANALY_TOKEN_TYPE() == TOKEN_NOT_EQUAL) {
        _rel_op(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        _add_expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _expr(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            expr ::= var '=' expr
                   | simple_expr
    */
    int idx, is_assign = 0;
    syntax_tree *node = NEW_AST_NODE("Expr", TOKEN_EXPR);

    if (ANALY_TOKEN_TYPE() == TOKEN_LEFT_ROUND_BRACKET ||
        ANALY_TOKEN_TYPE() == TOKEN_NUMBER) {
        _simple_expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        *ast = node;
        return;
    } else if (ANALY_TOKEN_TYPE() != TOKEN_ID) {
        invalid_token(ANALY_TOKEN_PTR());
    }

    if (ANALY_TOKEN_TYPE2(1) == TOKEN_LEFT_ROUND_BRACKET) {
        _simple_expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    } else {
        idx = *token_idx;
        _var(tokens, token_idx, &node->sub_list[node->sub_idx++]);

        if (ANALY_TOKEN_TYPE() == TOKEN_ASSIGN) {
            is_assign = 1;
        }

        // revert node
        free(node->sub_list[0]);
        node->sub_list[0] = NULL;
        node->sub_idx = 0;
        *token_idx = idx;

        if (is_assign) {
            _var(tokens, token_idx, &node->sub_list[node->sub_idx++]);
            _match_token(TOKEN_ASSIGN, tokens, token_idx);
            _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        } else {
            _simple_expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
        }
    }

    *ast = node;
}

void _expr_stmt(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            expr_stmt ::= [ expr ] ';'
    */

    if (ANALY_TOKEN_TYPE() == TOKEN_ID ||
        ANALY_TOKEN_TYPE() == TOKEN_LEFT_ROUND_BRACKET ||
        ANALY_TOKEN_TYPE() == TOKEN_NUMBER) {
        _expr(tokens, token_idx, ast);
    }

    _match_token(TOKEN_SEMICOLON, tokens, token_idx);
}

void _if_stmt(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            if_stmt ::= if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]
    */
    syntax_tree *node = NEW_AST_NODE("IfStmt", TOKEN_IF_STMT);

    _match_token(TOKEN_IF, tokens, token_idx);
    _match_token(TOKEN_LEFT_ROUND_BRACKET, tokens, token_idx);

    _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, tokens, token_idx);

    _match_token(TOKEN_LEFT_CURLY_BRACKET, tokens, token_idx);

    _stmt_list(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _match_token(TOKEN_RIGHT_CURLY_BRACKET, tokens, token_idx);

    if (ANALY_TOKEN_TYPE() == TOKEN_ELSE) {
        _match_token(TOKEN_ELSE, tokens, token_idx);
        _match_token(TOKEN_LEFT_CURLY_BRACKET, tokens, token_idx);

        _stmt_list(tokens, token_idx, &node->sub_list[node->sub_idx++]);

        _match_token(TOKEN_RIGHT_CURLY_BRACKET, tokens, token_idx);
    }

    *ast = node;
}

void _while_stmt(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            while_stmt ::= while '(' expr ')' stmt_list
    */
    syntax_tree *node = NEW_AST_NODE("WhileStmt", TOKEN_WHILE_STMT);

    _match_token(TOKEN_WHILE, tokens, token_idx);
    _match_token(TOKEN_LEFT_ROUND_BRACKET, tokens, token_idx);

    _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, tokens, token_idx);
    _match_token(TOKEN_LEFT_CURLY_BRACKET, tokens, token_idx);

    _stmt_list(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _match_token(TOKEN_RIGHT_CURLY_BRACKET, tokens, token_idx);

    *ast = node;
}

void _return_stmt(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            return_stmt ::= return [ expr ] ';'
    */
    syntax_tree *node = NEW_AST_NODE("ReturnStmt", TOKEN_RETURN_STMT);

    _match_token(TOKEN_RETURN, tokens, token_idx);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID ||
        ANALY_TOKEN_TYPE() == TOKEN_LEFT_ROUND_BRACKET ||
        ANALY_TOKEN_TYPE() == TOKEN_NUMBER) {
        _expr(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    _match_token(TOKEN_SEMICOLON, tokens, token_idx);

    *ast = node;
}

void _stmt(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            stmt ::= expr_stmt
                   | if_stmt
                   | while_stmt
                   | return_stmt
    */
    switch (ANALY_TOKEN_TYPE())
   {
        case TOKEN_SEMICOLON:
        case TOKEN_ID:
        case TOKEN_LEFT_ROUND_BRACKET:
        case TOKEN_NUMBER:
            _expr_stmt(tokens, token_idx, ast);
            break;
        case TOKEN_IF:
            _if_stmt(tokens, token_idx, ast);
            break;
        case TOKEN_WHILE:
            _while_stmt(tokens, token_idx, ast);
            break;
        case TOKEN_RETURN:
            _return_stmt(tokens, token_idx, ast);
            break;
        default:
            invalid_token(ANALY_TOKEN_PTR());
            break;
   }
}

void _stmt_list(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            stmt_list ::= { stmt }
    */

    syntax_tree *node = NEW_AST_NODE("StmtList", TOKEN_STMT_LIST);
    
    while (ANALY_TOKEN_TYPE() == TOKEN_SEMICOLON ||
        ANALY_TOKEN_TYPE() == TOKEN_ID ||
        ANALY_TOKEN_TYPE() == TOKEN_LEFT_ROUND_BRACKET ||
        ANALY_TOKEN_TYPE() == TOKEN_NUMBER ||
        ANALY_TOKEN_TYPE() == TOKEN_IF ||
        ANALY_TOKEN_TYPE() == TOKEN_WHILE ||
        ANALY_TOKEN_TYPE() == TOKEN_RETURN) {

        _stmt(tokens, token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

void _func_declared(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            func_declared ::= type id '(' params ')' '{' local_del stmt_list '}'
    */

    syntax_tree *node = NEW_AST_NODE("FuncDecl", TOKEN_FUNC_DECL);

    _type(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    if (ANALY_TOKEN_TYPE() == TOKEN_ID) {
        node->sub_list[node->sub_idx++] = NEW_AST_NODE3();
        _match_token(TOKEN_ID, tokens, token_idx);
    } else {
        invalid_token(ANALY_TOKEN_PTR());
    }

    _match_token(TOKEN_LEFT_ROUND_BRACKET, tokens, token_idx);

    _params(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _match_token(TOKEN_RIGHT_ROUND_BRACKET, tokens, token_idx);

    _match_token(TOKEN_LEFT_CURLY_BRACKET, tokens, token_idx);

    _local_declared(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _stmt_list(tokens, token_idx, &node->sub_list[node->sub_idx++]);

    _match_token(TOKEN_RIGHT_CURLY_BRACKET, tokens, token_idx);

    *ast = node;
}

void _declared(token_list *tokens, int *token_idx, syntax_tree **ast)
{
    /*
        EBNF:
            declared ::= local_declared
                    | func_declared
    */

    if (ANALY_TOKEN_TYPE() != TOKEN_INT &&
        ANALY_TOKEN_TYPE() != TOKEN_VOID) {
        invalid_token(ANALY_TOKEN_PTR());
    }

    if (ANALY_TOKEN_TYPE2(1) != TOKEN_ID) {
        invalid_token(ANALY_TOKEN_PTR2(1));
    }

    if (ANALY_TOKEN_TYPE2(2) == TOKEN_LEFT_SQUARE_BRACKET ||
        ANALY_TOKEN_TYPE2(2) == TOKEN_SEMICOLON) {
        _local_declared(tokens, token_idx, ast);
    } else if (ANALY_TOKEN_TYPE2(2) == TOKEN_LEFT_ROUND_BRACKET) {
        _func_declared(tokens, token_idx, ast);
    } else {
        invalid_token(ANALY_TOKEN_PTR2(2));
    }
}

void _declared_list(token_list *tokens, syntax_tree **ast)
{
    /*
        EBNF:
            declared_list ::= declared { declared }
    */
    int token_idx = 0;
    syntax_tree *node = NEW_AST_NODE("DeclList", TOKEN_DECL_LIST);

    while (token_idx < tokens->size) {
        _declared(tokens, &token_idx, &node->sub_list[node->sub_idx++]);
    }

    *ast = node;
}

int syntax_analysis(token_list *tokens, syntax_tree **ast)
{
    /*
        EBNF:
            program ::= declared_list
    */
    _declared_list(tokens, ast);
    return CMM_SUCCESS;
}