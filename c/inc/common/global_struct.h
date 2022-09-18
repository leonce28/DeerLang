#pragma once

#include <stdio.h>
#include "global_macro.h"

typedef struct _parse_handler {
    char input_cmm_path[FILE_PATH_MAX];
    char input_asm_path[FILE_PATH_MAX];
    char output_asm_path[FILE_PATH_MAX];
} parse_handler;

typedef struct _token {
    int token_type;
    int line_no;
    int token_len;
    char token_str[TOKEN_STR_MAX];
} token;

typedef struct _token_list {
    int capacity;
    int size;
    token **data;
} token_list;

// typedef struct _token_list_iterator {
//     token_list *tokens;
//     int token_idx;
// } tl_iterator;

typedef struct _lexical {
    char *str;
    int line_no;
    lexer_stage stage;
} lexical;

typedef struct _syntax_tree {
    int sub_idx;
    token *data;
    struct _syntax_tree **sub_list;
} syntax_tree;



