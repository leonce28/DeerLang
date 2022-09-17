#include <stdlib.h>
#include "front/syntax_analyzer.h"
#include "common/global_funcs.h"

static void _invalid_token(token *t)
{
    printf("invalid token: %s in line %d", t->token_str, t->line_no);
    exit(0);
}

void _var_declared(token *t, syntax_tree *ast)
{
    /*
        EBNF:
            var_declared ::= type id [ '[' number ']' ] ';'
    */

    // root = new AST(TokenType::VarDecl, "VarDecl", { nullptr, nullptr });

    // __Type(root->subList()[0], tokenPtr);

    // if (tokenPtr->tokenType() == TokenType::Id) {
    //     root->subList()[1] = new AST(tokenPtr);

    //     __matchToken(TokenType::Id, tokenPtr);
    // } else {
    //     __invalidToken(tokenPtr);
    // }

    // if (tokenPtr->tokenType() == TokenType::LeftSquareBracket) {
    //     __matchToken(TokenType::LeftSquareBracket, tokenPtr);

    //     root->subList().push_back(new AST(tokenPtr));

    //     __matchToken(TokenType::Number, tokenPtr);
    //     __matchToken(TokenType::RightSquareBracket, tokenPtr);
    // }

    // __matchToken(TokenType::Semicolon, tokenPtr);
}

void _func_declared(token *t, syntax_tree *ast)
{
    /*
        EBNF:
            func_declared ::= type id '(' param_list ')' compound_stmt
    */
    // root = new AST(TokenType::FuncDecl, "FuncDecl", { nullptr, nullptr, nullptr, nullptr });

    // __Type(root->subList()[0], tokenPtr);

    // if (tokenPtr->tokenType() == TokenType::Id) {
    //     root->subList()[1] = new AST(tokenPtr);

    //     __matchToken(TokenType::Id, tokenPtr);
    // } else {
    //     __invalidToken(tokenPtr);
    // }

    // __matchToken(TokenType::LeftRoundBracket, tokenPtr);

    // __Params(root->subList()[2], tokenPtr);

    // __matchToken(TokenType::RightRoundBracket, tokenPtr);

    // __CompoundStmt(root->subList()[3], tokenPtr);
}

void _declared(token *t, syntax_tree *ast)
{
    /*
        EBNF:
            declared ::= var_declared
                    | func_declared
    */

    if (t->token_type != TOKEN_INT && 
        t-> token_type != TOKEN_VOID) {
        _invalid_token(t);
    }

    token *t_1 = t + 1;
    if (t_1->token_type != TOKEN_INT) {
        _invalid_token(t_1);
    }

    token *t_2 = t + 2;
    if (t_2->token_type == TOKEN_LEFT_SQUARE_BRACKET ||
        t_2->token_type == TOKEN_SEMICOLON) {
        _var_declared(t, ast);
    } else if (t_2->token_type == TOKEN_LEFT_ROUND_BRACKET) {
        _func_declared(t, ast);
    } else {
        _invalid_token(t_2);
    }
}

void _declared_list(const token_list *tokens, syntax_tree *ast) 
{
    /*
        EBNF:
            declared_list ::= declared { declared }
    */
    token *t = tokens->data[0];
    _declared(t, ast);

    while (t->token_type != TOKEN_END) {
        syntax_tree_prepare(ast);
        _declared(t, ast);
   }

}

int syntax_analysis(const token_list *tokens, syntax_tree *ast)
{
    /*
        EBNF:
            Program ::= DeclList
    */
    _declared_list(tokens, ast);
    return CMM_SUCCESS;
}