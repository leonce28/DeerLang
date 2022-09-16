#pragma once

#include <stdio.h>
#include "global_macro.h"
#include "global_funcs.h"

typedef struct _parse_handler {
    char input_cmm_path[FILE_PATH_MAX];
    char input_asm_path[FILE_PATH_MAX];
    char output_asm_path[FILE_PATH_MAX];
} parse_handler;

typedef struct _funcs {
    funcs_init_fp init;
    funcs_push_fp push;
    funcs_print_fp print;
} funcs;

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
    funcs *op;
} token_list;

