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
    char space_name[SPACE_NAME_MAX];
    int s_idx;
    symbol **s;
} symbol_space;

typedef struct _symbol_table {
    int ss_idx;
    symbol_space **ss;
} symbol_table;

// struct code = { instruction ins, string offset }
//          => pair<__Instruction, string>
typedef struct _code {
    instruction ins; 
    char offset[VAR_OFFSET_MAX];
} code;

// struct code_list = { int c_idx, code c[] }
//          => vector<pair<__Instruction, string>
typedef struct _code_list {
    int c_idx;
    code **c;
} code_list;

// struct map_list = { char name[], code_list cl }
//          => <string, vector<pair<__Instruction, string>>
typedef struct _map_list {
    char name[MAP_NAME_MAX];
    code_list *cl;
} map_list;

// struct code_map = { int m_idx, map_list maps[] }
//          => unordered_map<string, vector<pair<__Instruction, string>>>
typedef struct _code_map {
    int m_idx;
    map_list **maps;
} code_map;

typedef struct _code_generator_handler {
    char size[VAR_SIZE_MAX];
    char *cur_space;
    const syntax_tree *tree;
    syntax_tree *node;
    const symbol_table *table;
    code_map *c_map;
    code_list *cl;
    code_list *g_cl;
} code_generator_handler;

