#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "kits/token_list.h"

int token_list_init(token_list *tokens)
{
    int idx;
    if (tokens == NULL) {
        printf("tokens is NULL.\n");
        return CMM_FAILED;
    }

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
    if (arg1 == NULL || arg2 == NULL) {
        printf("token_list_push param is null.\n");
        return CMM_FAILED;
    }

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

token *create_token(int no, char *str, int type)
{
    token *t = (token *)malloc(sizeof(token));
    memset(t, 0, sizeof(token));

    t->line_no = no;
    t->token_type = type;
    strcpy(t->token_str, str);
    return t;
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