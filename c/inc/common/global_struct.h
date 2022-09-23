#pragma once

#include <stdio.h>
#include "global_macro.h"

typedef struct _parse_handler {
    char cmm_file[FILE_PATH_MAX];
    char asm_file[FILE_PATH_MAX];
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

typedef struct _symbol {
    char var_name[VAR_NAME_MAX];
    int var_idx;
    int var_size;
} symbol;

typedef struct symbol_space {
    char space_name[FUNC_NAME_MAX];
    int s_idx;
    symbol **s;
} symbol_space;

typedef struct _symbol_table {
    int ss_idx;
    symbol_space **ss;
} symbol_table;

// struct code = { int instruction, string func_name }
//          => pair<__Instruction, string>
typedef struct _code {
    int instruction;
    char func_name[FUNC_NAME_MAX];
} code;

// struct code_list = { int c_idx, string map_name, code c[] >
//          => <string, vector<pair<__Instruction, string>>
typedef struct _code_map {
    int c_idx;
    char map_name[MAP_NAME_MAX];
    code **c;
} code_map;

// struct unordered_code_map = { int m_idx, code_map maps[] }
//          => unordered_map<string, vector<pair<__Instruction, string>>>
typedef struct _unordered_code_map {
    int m_idx;
    code_map **maps;
} unordered_code_map;

