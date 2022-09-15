#include <stdio.h>
#include <stdlib.h>
#include "kits/token_list.h"

int token_list_init(token_list *tokens)
{
    int idx;
    if (tokens == NULL) {
        return CMM_FAILED;
    }
    tokens->capacity = TOKEN_LIST_MAX;
    tokens->size = 0;
    tokens->op->push = NULL;
    tokens->data = (token **)malloc(TOKEN_LIST_MAX * sizeof(token *));
    if (tokens->data == NULL) {
        return CMM_FAILED;
    }

    for (idx = 0; idx < tokens->capacity; ++idx) {
        tokens->data[idx] = (token *)malloc(sizeof(token));
        if (tokens->data[idx] == NULL) {
            return CMM_FAILED;
        }
    }

    return CMM_SUCCESS;
}