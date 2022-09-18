#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common/global_funcs.h"

static void _token_print(token *t)
{
    assert(t != NULL);

    printf("\t\t\tline_no: %d,\n\t\t\ttoken_str: \"%s\",\n\t\t\ttoken_len: %d,\n\t\t\ttoken_type: %d\n", 
    t->line_no, t->token_str, t->token_len, t->token_type);
}

void token_print(const token_list *tokens, int token_idx)
{
    assert(tokens != NULL);

    token *t = tokens->data[token_idx];
    printf("%d: { line_no: %d, token_str: \"%s\", token_len: %d, token_type: %d }\n", 
        token_idx, t->line_no, t->token_str, 
        t->token_len, t->token_type);
}

token *create_token(const char *str, int type)
{
    token *t = (token *)malloc(sizeof(token));
    if (t == NULL) {
        return NULL;
    }

    memset(t, 0, sizeof(token));
    t->token_type = type;
    t->token_len = 0;
    t->line_no = 0;

    if (str != NULL) {
        t->token_len = MIN(TOKEN_STR_MAX, strlen(str));
        strncpy(t->token_str, str, t->token_len);
    }
    return t;
}

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

    tokens->capacity = TOKEN_LIST_MAX;
    tokens->size = 0;

    return CMM_SUCCESS;
}

int token_list_push(token_list *tl, token *t)
{
    assert(tl != NULL && t != NULL);

    tl->data[tl->size++] = t;
    if (tl->size >= tl->capacity) {
        printf("insufficient free space, please expand.\n");
        // todo
        return CMM_FAILED;
    }

    return CMM_SUCCESS;
}

void token_list_print(const token_list *tl)
{
    assert(tl != NULL);

    int idx;
    printf("token_list: [\n");
    for (idx = 0; idx < tl->size; ++idx) {
        printf("\t{\n\t\tidx: %d,\n\t\ttoken: {\n", idx);
        _token_print(tl->data[idx]);
        printf("\t\t}\n\t}");
        if (idx != tl->size - 1) {
            printf(",");
        }
        printf("\n");
    }

    printf("]\n");
}

void token_push_char(token *t, char ch)
{
    assert(t != NULL);

    if (t->token_len >= TOKEN_STR_MAX) {
        printf("token length is too long for token_str array, max length: %d.\n", TOKEN_STR_MAX);
        exit(0);
    }

    t->token_str[t->token_len++] = ch;
}

syntax_tree *create_ast_node(token *t)
{
    syntax_tree *node = (syntax_tree *)malloc(sizeof(syntax_tree));
    memset(node, 0, sizeof(syntax_tree));
    node->data = t;
    node->sub_list = (syntax_tree **)malloc(AST_LIST_MAX * sizeof(syntax_tree *));
    node->sub_idx = 0;

    return node;
}

static void _ast_print_node(const syntax_tree *node, int deep)
{
    assert(node != NULL);
    int tab_size = deep;
    while (tab_size-- > 0) {
        printf("   ");
    }
    printf("deep: %d, sub_size: %d, token: ", deep, node->sub_idx);
    printf("{ line_no: %d, token_str: \"%s\", token_len: %d, token_type: %d }\n",
        node->data->line_no, node->data->token_str,
        node->data->token_len, node->data->token_type);
}

static void _syntax_tree_print(const syntax_tree *node, int deep)
{
    int si;
    if (node != NULL) {
        _ast_print_node(node, deep);
        for (si = 0; si < node->sub_idx; ++si) {
            if (node->sub_list[si] != NULL) {
                _syntax_tree_print(node->sub_list[si], deep + 1);
            }
        }
    }
}

void syntax_tree_print(const syntax_tree *ast)
{   
    int deep = 0;
    _syntax_tree_print(ast, deep);
}
