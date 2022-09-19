#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "front/lexical_analyzer.h"
#include "common/global_macro.h"
#include "common/global_funcs.h"

static int _read_code_string(const char *cmm_file_path, char **str)
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

    *str = (char *)malloc(file_length);
    if ((*str) == NULL) {
        printf("malloc code string failed.\n");
        return CMM_FAILED;
    }

    if (count != fread((*str), file_length, count, fp)) {
        printf("fread cmm file failed, file path: %s.\n", cmm_file_path);
        return CMM_FAILED;
    }

    fclose(fp);

    return CMM_SUCCESS;
}

static int _get_token_list_instance(token_list **tl)
{
    token_list *tokens = (token_list *)malloc(sizeof(token_list));
    if (!tokens) {
        return CMM_FAILED;
    }
    memset(tokens, 0, sizeof(token_list));
    (*tl) = tokens;
    return token_list_init(tokens);
}

static void _invalid_char(char ch, int line_no)
{
    printf("invalid char: %c in line: %d", ch, line_no);
    exit(0);
}

static void _invalid_stage(lexer_stage stage)
{
    printf("invalid lexer stage value: %d, exit.", stage);
    exit(0);
}

static void _next_stage_start(lexical *lex, token *t)
{
    if (isalpha(*lex->str)) {
        lex->stage = STAGE_IN_ID;
        token_push_char(t, *lex->str++);
    } else if (isdigit(*lex->str)) {
        lex->stage = STAGE_IN_NUMBER;
        token_push_char(t, *lex->str++);
    } else if (isspace(*lex->str)) {
        if (*lex->str == '\n') {
            lex->line_no++; 
        }
        lex->str++;
    } else {
        switch (*lex->str) {
            case '+':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_PLUS;
                token_push_char(t, *lex->str++);
                break;
            case '-':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_MINUS;
                token_push_char(t, *lex->str++);
                break;
            case '*':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_MULTIPLY;
                token_push_char(t, *lex->str++);
                break;
            case '/':
                lex->stage = STAGE_IN_DIVIDE;
                lex->str++;
                break;
            case '<':
                lex->stage = STAGE_IN_LESS;
                token_push_char(t, *lex->str++);
                break;
            case '>':
                lex->stage = STAGE_IN_GREATER;
                token_push_char(t, *lex->str++);
                break;
            case '=':
                lex->stage = STAGE_IN_ASSIGN;
                token_push_char(t, *lex->str++);
                break;
            case '!':
                lex->stage = STAGE_IN_NOT;
                token_push_char(t, *lex->str++);
                break;
            case ';':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_SEMICOLON;
                token_push_char(t, *lex->str++);
                break;
            case ',':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_COMMA;
                token_push_char(t, *lex->str++);
                break;
            case '(':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_LEFT_ROUND_BRACKET;
                token_push_char(t, *lex->str++);
                break;
            case ')':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_RIGHT_ROUND_BRACKET;
                token_push_char(t, *lex->str++);
                break;
            case '[':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_LEFT_SQUARE_BRACKET;
                token_push_char(t, *lex->str++);
                break;
            case ']':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_RIGHT_SQUARE_BRACKET;
                token_push_char(t, *lex->str++);
                break;
            case '{':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_LEFT_CURLY_BRACKET;
                token_push_char(t, *lex->str++);
                break;
            case '}':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_RIGHT_CURLY_BRACKET;
                token_push_char(t, *lex->str++);
                break;
            case '\0':
                lex->stage = STAGE_DONE;
                t->token_type = TOKEN_END;
                break;
            default:
                _invalid_char(*lex->str, t->line_no);
                break;
        }
    }
}

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

static void _next_stage_in_id(lexical *lex, token *t)
{
    if (isalpha(*lex->str)) {
        token_push_char(t, *lex->str++);
    } else {
        lex->stage = STAGE_DONE;
        _set_token_id_type(t);
    }
}

static void _next_stage_in_number(lexical *lex, token *t)
{
    if (isdigit(*lex->str)) {
        token_push_char(t, *lex->str++);
    } else {
        lex->stage = STAGE_DONE;
        t->token_type = TOKEN_NUMBER;
    }
}

static void _next_stage_in_divide(lexical *lex, token *t)
{
    if (*lex->str == '*') {
        lex->stage = STAGE_IN_COMMENT;
        lex->str++;
    } else {
        lex->stage = STAGE_DONE;
        t->token_type = TOKEN_DIVIDE;
        token_push_char(t, '/');
    }
}

static void _next_stage_in_comment(lexical *lex, token *t)
{
    if (*lex->str == '*') {
        lex->stage = STAGE_END_COMMENT;
    } else if (*lex->str == '\n') {
        lex->line_no++; 
    }

    lex->str++;
}

static void _next_stage_end_comment(lexical *lex, token *t)
{
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

static void _next_stage_in_less(lexical *lex, token *t)
{
    lex->stage = STAGE_DONE;

    if (*lex->str == '=') {
        t->token_type = TOKEN_LESS_EQUAL;
        token_push_char(t, *lex->str++);
    } else {
        t->token_type = TOKEN_LESS;
    }
}

static void _next_stage_in_greater(lexical *lex, token *t)
{
    lex->stage = STAGE_DONE;

    if (*lex->str == '=') {
        t->token_type = TOKEN_GREATER_EQUAL;
        token_push_char(t, *lex->str++);
    } else {
        t->token_type = TOKEN_GREATER;
    }
}

static void _next_stage_in_assign(lexical *lex, token *t)
{
    lex->stage = STAGE_DONE;

    if (*lex->str == '=') {
        t->token_type = TOKEN_EQUAL;
        token_push_char(t, *lex->str++);
    } else {
        t->token_type = TOKEN_ASSIGN;
    }
}

static void _next_stage_in_not(lexical *lex, token *t)
{
    if (*lex->str == '=') {
        lex->stage = STAGE_DONE;
        t->token_type = TOKEN_NOT_EQUAL;
        token_push_char(t, *lex->str++);
    } else {
        _invalid_char(*lex->str, t->line_no);
    }
}

static token *_next_token(lexical *lex)
{
    token *t = create_token(NULL, TOKEN_END);
    while (lex->stage != STAGE_DONE) {
        switch (lex->stage) {
            case STAGE_START:
                _next_stage_start(lex, t);
                break;
            case STAGE_IN_ID:
                _next_stage_in_id(lex, t);
                break;
            case STAGE_IN_NUMBER:
                _next_stage_in_number(lex, t);
                break;
            case STAGE_IN_DIVIDE:
                _next_stage_in_divide(lex, t);
                break;
            case STAGE_IN_COMMENT:
                _next_stage_in_comment(lex, t);
                break;
            case STAGE_END_COMMENT:
                _next_stage_end_comment(lex, t);
                break;
            case STAGE_IN_LESS:
                _next_stage_in_less(lex, t);
                break;
            case STAGE_IN_GREATER:
                _next_stage_in_greater(lex, t);
                break;
            case STAGE_IN_ASSIGN:
                _next_stage_in_assign(lex, t);
                break;
            case STAGE_IN_NOT:
                _next_stage_in_not(lex, t);
                break;
            default:
                _invalid_stage(lex->stage);
                break;
        }
    }

    t->line_no = lex->line_no;
    return t;
}

int lexical_analysis(const char *cmm_file_path, token_list **tl_ptr)
{
    token *t;
    char *code_str;
    lexical lex = { NULL, 1 , STAGE_START };

    if (CMM_SUCCESS != _get_token_list_instance(tl_ptr)) {
        printf("get token list instance failed. \n");
        return CMM_FAILED;
    }

    if (CMM_SUCCESS != _read_code_string(cmm_file_path, &lex.str) || lex.str == NULL) {
        printf("lexical analysis read code string failed. \n");
        return CMM_FAILED;
    }

    code_str = lex.str;
    t = _next_token(&lex);
    while (t != NULL && t->token_type != TOKEN_END) {
        token_list_push(*tl_ptr, t);
        lex.stage = STAGE_START;
        t = _next_token(&lex);
    }

    free(code_str);
    return CMM_SUCCESS;
}

