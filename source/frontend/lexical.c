#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common/macro.h"
#include "common/funcs.h"
#include "common/list.h"

#include "frontend/lexical.h"

static void invalid_char(char ch, int line_no)
{
    printf("invalid char: %c in line: %d", ch, line_no);
    exit(0);
}

static void invalid_stage(LexicalStage stage)
{
    printf("invalid lexer stage value: %d, exit.", stage);
    exit(0);
}

static void next_stage_start(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (isalpha(*lex->str)) {
        lex->stage = STAGE_IN_ID;
        token_push_char(token, *lex->str++);
    } else if (isdigit(*lex->str)) {
        lex->stage = STAGE_IN_NUMBER;
        token_push_char(token, *lex->str++);
    } else if (isspace(*lex->str)) {
        if (*lex->str == '\n') {
            lex->line_no++; 
        }
        lex->str++;
    } else {
        switch (*lex->str) {
            case '+':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_PLUS;
                token_push_char(token, *lex->str++);
                break;
            case '-':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_MINUS;
                token_push_char(token, *lex->str++);
                break;
            case '*':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_MULTIPLY;
                token_push_char(token, *lex->str++);
                break;
            case '/':
                lex->stage = STAGE_IN_DIVIDE;
                lex->str++;
                break;
            case '<':
                lex->stage = STAGE_IN_LESS;
                token_push_char(token, *lex->str++);
                break;
            case '>':
                lex->stage = STAGE_IN_GREATER;
                token_push_char(token, *lex->str++);
                break;
            case '=':
                lex->stage = STAGE_IN_ASSIGN;
                token_push_char(token, *lex->str++);
                break;
            case '!':
                lex->stage = STAGE_IN_NOT;
                token_push_char(token, *lex->str++);
                break;
            case ';':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_SEMICOLON;
                token_push_char(token, *lex->str++);
                break;
            case ',':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_COMMA;
                token_push_char(token, *lex->str++);
                break;
            case '(':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_LEFT_ROUND_BRACKET;
                token_push_char(token, *lex->str++);
                break;
            case ')':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_RIGHT_ROUND_BRACKET;
                token_push_char(token, *lex->str++);
                break;
            case '[':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_LEFT_SQUARE_BRACKET;
                token_push_char(token, *lex->str++);
                break;
            case ']':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_RIGHT_SQUARE_BRACKET;
                token_push_char(token, *lex->str++);
                break;
            case '{':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_LEFT_CURLY_BRACKET;
                token_push_char(token, *lex->str++);
                break;
            case '}':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_RIGHT_CURLY_BRACKET;
                token_push_char(token, *lex->str++);
                break;
            case '\0':
                lex->stage = STAGE_DONE;
                token->type = TOKEN_END;
                break;
            default:
                invalid_char(*lex->str, token->line_no);
                break;
        }
    }
}

static void set_token_id_type(DeerToken *token)
{
    assert(token);

    if (strcmp(token->token_str, "void") == 0) {
        token->type = TOKEN_VOID;
    } else if (strcmp(token->token_str, "int") == 0) {
        token->type = TOKEN_INT;
    } else if (strcmp(token->token_str, "if") == 0) {
        token->type = TOKEN_IF;
    } else if (strcmp(token->token_str, "else") == 0) {
        token->type = TOKEN_ELSE;
    } else if (strcmp(token->token_str, "while") == 0) {
        token->type = TOKEN_WHILE;
    } else if (strcmp(token->token_str, "return") == 0) {
        token->type = TOKEN_RETURN;
    } else {
        token->type = TOKEN_ID;
    }
}

static void next_stage_in_id(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (isalpha(*lex->str)) {
        token_push_char(token, *lex->str++);
    } else {
        lex->stage = STAGE_DONE;
        set_token_id_type(token);
    }
}

static void next_stage_in_number(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (isdigit(*lex->str)) {
        token_push_char(token, *lex->str++);
    } else {
        lex->stage = STAGE_DONE;
        token->type = TOKEN_NUMBER;
    }
}

static void next_stage_in_divide(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (*lex->str == '*') {
        lex->stage = STAGE_IN_COMMENT;
        lex->str++;
    } else {
        lex->stage = STAGE_DONE;
        token->type = TOKEN_DIVIDE;
        token_push_char(token, '/');
    }
}

static void next_stage_in_comment(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (*lex->str == '*') {
        lex->stage = STAGE_END_COMMENT;
    } else if (*lex->str == '\n') {
        lex->line_no++; 
    }

    lex->str++;
}

static void next_stage_end_comment(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (*lex->str == '/') {
        lex->stage = STAGE_START;
    } else if (*lex->str != '*') {
        lex->stage = STAGE_IN_COMMENT;
        if (*lex->str == '\n') {
            lex->line_no++; 
        }
    }

    lex->str++;
}

static void next_stage_in_less(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    lex->stage = STAGE_DONE;

    if (*lex->str == '=') {
        token->type = TOKEN_LESS_EQUAL;
        token_push_char(token, *lex->str++);
    } else {
        token->type = TOKEN_LESS;
    }
}

static void next_stage_in_greater(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    lex->stage = STAGE_DONE;

    if (*lex->str == '=') {
        token->type = TOKEN_GREATER_EQUAL;
        token_push_char(token, *lex->str++);
    } else {
        token->type = TOKEN_GREATER;
    }
}

static void next_stage_in_assign(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    lex->stage = STAGE_DONE;

    if (*lex->str == '=') {
        token->type = TOKEN_EQUAL;
        token_push_char(token, *lex->str++);
    } else {
        token->type = TOKEN_ASSIGN;
    }
}

static void next_stage_in_not(DeerLexical *lex, DeerToken *token)
{
    assert(lex && token);

    if (*lex->str == '=') {
        lex->stage = STAGE_DONE;
        token->type = TOKEN_NOT_EQUAL;
        token_push_char(token, *lex->str++);
    } else {
        invalid_char(*lex->str, token->line_no);
    }
}

static DeerToken *lexical_next(DeerLexical *lex)
{
    assert(lex);

    DeerToken *token = create_token(nullptr, TOKEN_END);

    while (lex->stage != STAGE_DONE) {
        switch (lex->stage) {
            case STAGE_START:
                next_stage_start(lex, token);
                break;
            case STAGE_IN_ID:
                next_stage_in_id(lex, token);
                break;
            case STAGE_IN_NUMBER:
                next_stage_in_number(lex, token);
                break;
            case STAGE_IN_DIVIDE:
                next_stage_in_divide(lex, token);
                break;
            case STAGE_IN_COMMENT:
                next_stage_in_comment(lex, token);
                break;
            case STAGE_END_COMMENT:
                next_stage_end_comment(lex, token);
                break;
            case STAGE_IN_LESS:
                next_stage_in_less(lex, token);
                break;
            case STAGE_IN_GREATER:
                next_stage_in_greater(lex, token);
                break;
            case STAGE_IN_ASSIGN:
                next_stage_in_assign(lex, token);
                break;
            case STAGE_IN_NOT:
                next_stage_in_not(lex, token);
                break;
            default:
                invalid_stage(lex->stage);
                break;
        }
    }

    token->line_no = lex->line_no;
    return token;
}

int lexical_analysis(DeerCompilerHandle *handle)
{
    assert(handle);

    DeerToken *token = nullptr;
    DeerLinkedList *token_list = nullptr;

    DeerLexical lex = {
        .str = handle->file_content,
        .line_no = 1,       // begin at line 1.
        .stage = STAGE_START
    };

    while ((token = lexical_next(&lex)) != nullptr && 
            token->type != TOKEN_END) {
        token_list = dlist_push_back(token_list, token);
        lex.stage = STAGE_START;
    }

    handle->tokens = token_list;
    return CMM_SUCCESS;
}

