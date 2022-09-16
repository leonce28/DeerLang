#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "front/lexical_analysis.h"
#include "common/global_macro.h"

static int _read_code_string(const char *cmm_file_path, char **code_str)
{
    FILE *fp;
    size_t file_length, count = 1;

    fp = fopen(cmm_file_path, "r");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", cmm_file_path);
        return CMM_FAILED;
    }

    fseek(fp, 0, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *code_str = (char *)malloc(file_length);
    if ((*code_str) == NULL) {
        printf("malloc code string failed.\n");
        return CMM_FAILED;
    }

    if (count != fread((*code_str), file_length, count, fp)) {
        printf("fread cmm file failed, file path: %s.\n", cmm_file_path);
        return CMM_FAILED;
    }

    fclose(fp);

    return CMM_SUCCESS;
}

static void _invalid_char(char ch, int line_no)
{
    printf("invalid char: %c in line: %d", ch, line_no);
    exit(0);
}

static void _next_stage_start(const char *code_str, lexer_stage *stage, token *t)
{
    if (isalpha(*code_str)) {
        *stage = STAGE_IN_ID;
        token_push_char(t, *code_str++);
    } else if (isdigit(*code_str)) {
        *stage = STAGE_IN_NUMBER;
        token_push_char(t, *code_str++);
    } else if (isspace(*code_str)) {
        if (*code_str == '\n') {
            t->line_no++;
        }
        code_str++;
    } else {
        switch (*code_str) {
            case '+':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_PLUS;
                code_str++;
                break;
            case '-':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_MINUS;
                code_str++;
                break;
            case '*':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_MULTIPLY;
                code_str++;
                break;
            case '/':
                *stage = STAGE_IN_DIVIDE;
                code_str++;
                break;
            case '<':
                *stage = STAGE_IN_LESS;
                code_str++;
                break;
            case '>':
                *stage = STAGE_IN_GREATER;
                code_str++;
                break;
            case '=':
                *stage = STAGE_IN_ASSIGN;
                code_str++;
                break;
            case '!':
                *stage = STAGE_IN_NOT;
                code_str++;
                break;
            case ';':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_SEMICOLON;
                code_str++;
                break;
            case ',':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_COMMA;
                code_str++;
                break;
            case '(':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_LEFT_ROUND_BRACKET;
                code_str++;
                break;
            case ')':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_RIGHT_ROUND_BRACKET;
                code_str++;
                break;
            case '[':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_LEFT_SQUARE_BRACKET;
                code_str++;
                break;
            case ']':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_RIGHT_SQUARE_BRACKET;
                code_str++;
                break;
            case '{':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_LEFT_CURLY_BRACKET;
                code_str++;
                break;
            case '}':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_RIGHT_CURLY_BRACKET;
                code_str++;
                break;
            case '\0':
                *stage = STAGE_DONE;
                t->token_type = TOKEN_END;
                break;
            default:
                _invalid_char(*code_str, t->line_no);
                break;
        }
    }
}

/*
const unordered_map<string, TokenType> KEYWORD_MAP
{
    {"void",   TokenType::Void},
    {"int",    TokenType::Int},
    {"if",     TokenType::If},
    {"else",   TokenType::Else},
    {"while",  TokenType::While},
    {"return", TokenType::Return},
};
*/

static void _set_token_id_type(token *t)
{
    if (strcmp(t->token_str, "void") == 0) {
        t->token_type = TOKEN_VOID;
    } else if (strcmp(t->token_str, "int") == 0) {
        t->token_type = TOKEN_INT;
    } else if (strcmp(t->token_str, "if") == 0) {
        t->token_type = TOKEN_IF;
    } else if (strcmp(t->token_str, "else") == 0) {
        t->token_type = TOKEN_ELSE;
    } else if (strcmp(t->token_str, "while") == 0) {
        t->token_type = TOKEN_WHILE;
    } else if (strcmp(t->token_str, "return") == 0) {
        t->token_type = TOKEN_RETURN;
    } else {
        t->token_type = TOKEN_ID;
    }
}

static void _next_stage_in_id(const char *code_str, lexer_stage *stage, token *t)
{
    if (isalpha(*code_str)) {
        token_push_char(t, *code_str++);
    } else {
        *stage = STAGE_DONE;
        _set_token_id_type(t);
    }
}

static void _next_stage_in_number(const char *code_str, lexer_stage *stage, token *t)
{
    if (isdigit(*code_str)) {
        token_push_char(t, *code_str++);
    } else {
        *stage = STAGE_DONE;
        t->token_type = TOKEN_NUMBER;
    }
}

static void _next_stage_in_divide(const char *code_str, lexer_stage *stage, token *t)
{
    if (*code_str == '*') {
        *stage = STAGE_IN_COMMENT;
        code_str++;
    } else {
        *stage = STAGE_DONE;
        t->token_type = TOKEN_DIVIDE;
        token_push_char(t, '/');
    }
}

static void _next_stage_in_comment(const char *code_str, lexer_stage *stage, token *t)
{
    if (*code_str == '*') {
        *stage = STAGE_END_COMMENT;
    } else if (*code_str == '\n') {
        t->line_no++;
    }

    code_str++;
}

static void _next_stage_end_comment(const char *code_str, lexer_stage *stage, token *t)
{
    if (*code_str == '/') {
        *stage = STAGE_START;
    } else if (*code_str != '*') {
        *stage = STAGE_IN_COMMENT;
        if (*code_str == '\n') {
            t->line_no++;
        }
    }
}

static void _next_stage_in_less(const char *code_str, lexer_stage *stage, token *t)
{
    *stage = STAGE_DONE;

    if (*code_str == '=') {
        t->token_type = TOKEN_LESS_EQUAL;
        token_push_char(t, *code_str++);
    } else {
        t->token_type = TOKEN_LESS;
    }
}

static void _next_stage_in_greater(const char *code_str, lexer_stage *stage, token *t)
{
    *stage = STAGE_DONE;

    if (*code_str == '=') {
        t->token_type = TOKEN_GREATER_EQUAL;
        token_push_char(t, *code_str++);
    } else {
        t->token_type = TOKEN_GREATER;
    }
}

static void _next_stage_in_assign(const char *code_str, lexer_stage *stage, token *t)
{
    *stage = STAGE_DONE;

    if (*code_str == '=') {
        t->token_type = TOKEN_EQUAL;
        token_push_char(t, *code_str++);
    } else {
        t->token_type = TOKEN_ASSIGN;
    }
}

static void _next_stage_in_not(const char *code_str, lexer_stage *stage, token *t)
{
    if (*code_str == '=') {
        *stage = STAGE_DONE;
        t->token_type = TOKEN_NOT_EQUAL;
        token_push_char(t, *code_str++);
    } else {
        _invalid_char(*code_str, t->line_no);
    }
}

static token *_next_token(const char *code_str, int line_no)
{
    lexer_stage stage = STAGE_START;
    token *t = create_token(line_no, NULL, TOKEN_END);
    while (stage != STAGE_DONE) {
        switch (stage) {
            case STAGE_START:
                _next_stage_start(code_str, &stage, t);
                break;
            case STAGE_IN_ID:
                _next_stage_in_id(code_str, &stage, t);
                break;
            case STAGE_IN_NUMBER:
                _next_stage_in_number(code_str, &stage, t);
                break;
            case STAGE_IN_DIVIDE:
                _next_stage_in_divide(code_str, &stage, t);
                break;
            case STAGE_IN_COMMENT:
                _next_stage_in_comment(code_str, &stage, t);
                break;
            case STAGE_END_COMMENT:
                _next_stage_end_comment(code_str, &stage, t);
                break;
            case STAGE_IN_LESS:
                _next_stage_in_less(code_str, &stage, t);
                break;
            case STAGE_IN_GREATER:
                _next_stage_in_greater(code_str, &stage, t);
                break;
            case STAGE_IN_ASSIGN:
                _next_stage_in_assign(code_str, &stage, t);
                break;
            case STAGE_IN_NOT:
                _next_stage_in_not(code_str, &stage, t);
                break;
            default:
                printf("invalid lexer stage value: %d, exit.", stage);
                exit(0);
        }
    }

    printf("token: { line_no: %d, token_str: %s, token_type: %d } \n", t->line_no, t->token_str, t->token_type);
    return t;
}

int lexical_analysis(const char *cmm_file_path, token_list *tokens)
{
    char *code_str;
    int line_no = 1;
    token *t;
    if (CMM_SUCCESS != _read_code_string(cmm_file_path, &code_str) || code_str == NULL) {
        printf("lexical analysis read code string failed. \n");
        return CMM_FAILED;
    }

    t = _next_token(code_str, line_no);
    while (t != NULL && t->token_type != TOKEN_END) {
        tokens->op->push(tokens, t);
        t = _next_token(code_str, line_no);
    }

    return CMM_SUCCESS;
}

