#include <stdlib.h>
#include "front/syntax_analyzer.h"
#include "common/global_funcs.h"

static void _invalid_token(token *t)
{
    printf("invalid token: %s in line %d", t->token_str, t->line_no);
    exit(0);
}

void _match_token(int type, analyzer *analy)
{
    if (ANALY_TOKEN_TYPE(analy) == type) {
        analy->token_idx++;
    } else {
        _invalid_token(ANALY_TOKEN_PTR(analy));
    }
}

void _type(analyzer *analy)
{
    /*
        EBNF:
            Type ::= int
                | void
    */
   
    if (ANALY_TOKEN_TYPE(analy) == TOKEN_INT ||
        ANALY_TOKEN_TYPE(analy) == TOKEN_VOID) {
        NEW_AST_NODE(analy->ast, ANALY_TOKEN_PTR(analy)->token_str, ANALY_TOKEN_PTR(analy)->token_type);
        _match_token(ANALY_TOKEN_TYPE(analy), analy);
    } else {
        _invalid_token(ANALY_TOKEN_PTR(analy));
    }
}

void _var_declared(analyzer *analy)
{
    /*
        EBNF:
            var_declared ::= type id [ '[' number ']' ] ';'
    */
    NEW_AST_NODE(analy->ast, "VarDecl", TOKEN_VAR_DECL);

    _type(analy);;

    if (ANALY_TOKEN_TYPE(analy) == TOKEN_ID) {

    }

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

void _func_declared(analyzer *analy)
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

void _declared(analyzer *analy)
{
    /*
        EBNF:
            declared ::= var_declared
                    | func_declared
    */
    token *t = analy->tokens->data[analy->token_idx];
    token *t_1 = analy->tokens->data[analy->token_idx + 1];
    token *t_2 = analy->tokens->data[analy->token_idx + 2];

    if (t->token_type != TOKEN_INT && 
        t->token_type != TOKEN_VOID) {
        _invalid_token(t);
    }

    if (t_1->token_type != TOKEN_INT) {
        _invalid_token(t_1);
    }

    if (t_2->token_type == TOKEN_LEFT_SQUARE_BRACKET ||
        t_2->token_type == TOKEN_SEMICOLON) {
        _var_declared(analy);
    } else if (t_2->token_type == TOKEN_LEFT_ROUND_BRACKET) {
        _func_declared(analy);
    } else {
        _invalid_token(t_2);
    }
}

void _declared_list(analyzer *analy) 
{
    /*
        EBNF:
            declared_list ::= declared { declared }
    */
   NEW_AST_NODE(analy->ast, "DeclList", TOKEN_DECL_LIST);
    while (analy->tokens->data[analy->token_idx]->token_type != TOKEN_END) {
        token_print(analy->tokens->data[analy->token_idx]);
        MOVE_AST_NODE(analy);
        _declared(analy);
   }

}

int syntax_analysis(token_list *tokens, syntax_tree **ast)
{
    /*
        EBNF:
            Program ::= DeclList
    */
    analyzer analy = { tokens, *ast, 0 };
    _declared_list(&analy);
    return CMM_SUCCESS;
}