#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "kits/token_list.h"

int token_list_init(token_list *tokens)
{
    assert(tokens != NULL);

    int idx;
    tokens->data = (token **)malloc(TOKEN_LIST_MAX * sizeof(token *));
    if (tokens->data == NULL) {
        printf("tokens->data is NULL.\n");
        return CMM_FAILED;
    }

    for (idx = 0; idx < tokens->capacity; ++idx) {
        tokens->data[idx] = (token *)malloc(sizeof(token));
        if (tokens->data[idx] == NULL) {
            printf("tokens->data[idx] is NULL.\n");
            return CMM_FAILED;
        }
    }

    tokens->op = (funcs *)malloc(sizeof(funcs));;
    if (tokens->op == NULL) {
        printf("tokens->op is NULL.\n");
        return CMM_FAILED;
    }

    tokens->op->push = token_list_push;
    tokens->op->print = token_list_print;
    tokens->capacity = TOKEN_LIST_MAX;
    tokens->size = 0;

    return CMM_SUCCESS;
}

int token_list_push(void *arg1, void *arg2)
{
    assert(arg1 != NULL && arg2 != NULL);

    token_list *tl = (token_list *)arg1;
    token *t = (token *)arg2;

    tl->data[tl->size++] = t;
    if (tl->size >= tl->capacity) {
        printf("insufficient free space, please expand.\n");
        // todo
        return CMM_FAILED;
    }

    return CMM_SUCCESS;
}

void print_token(token *t)
{
    assert(t != NULL);

    printf("{ no: %d, str: %s, type: %d }", t->line_no, t->token_str, t->token_type);
}

token *create_token(int line_no, const char *str, int type)
{
    token *t = (token *)malloc(sizeof(token));
    memset(t, 0, sizeof(token));

    t->line_no = line_no;
    t->token_type = type;
    t->token_len = 0;

    if (str != NULL) {
        t->token_len = MIN(TOKEN_STR_MAX, strlen(str));
        strncpy(t->token_str, str, t->token_len);
    }
    return t;
}

void token_push_char(token *t, char ch)
{
    assert(t != NULL);
    
    if (t->token_len >= TOKEN_STR_MAX) {
        printf("token length is too long for token_str array, max length: %d.\n", TOKEN_STR_MAX);
        exit(0);
    }

    t->token_str[t->token_len] = ch;
}

int token_list_print(void *arg1)
{
    assert(arg1 != NULL);

    int idx;
    token_list *tl = (token_list *)arg1;
    for (idx = 0; idx < tl->size; ++idx) {
        printf("idx: %d, token: ", idx);
        print_token(tl->data[idx]);
        printf("\n");
    }

    return CMM_SUCCESS;
}