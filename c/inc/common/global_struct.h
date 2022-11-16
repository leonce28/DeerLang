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

// pair<string, <int, int>>
typedef struct _symbol {
    char var_name[VAR_NAME_MAX];
    int var_idx;
    int var_size;
} symbol;

// pair<string, pair<string, <int, int>>>
typedef struct symbol_space {
    char space_name[SPACE_NAME_MAX];
    int s_idx;
    symbol **s;
} symbol_space;

// vector<pair<string, pair<string, <int, int>>>>
typedef struct _symbol_table {
    int ss_idx;
    symbol_space **ss;
} symbol_table;

// pair<int, string>
typedef struct _code {
    instruction ins; 
    char offset[VAR_OFFSET_MAX];
} code;

// vector<pair<int, string>
typedef struct _code_list {
    int c_idx;
    code **c;
} code_list;

// <string, vector<pair<int, string>>
typedef struct _map_list {
    char name[MAP_NAME_MAX];
    code_list *cl;
} map_list;

// unordered_map<string, vector<pair<int, string>>>
typedef struct _code_map {
    int m_idx;
    map_list **maps;
} code_map;

// Function name => Function start IP
typedef struct _func_jump {
    char *name;
    int jump_num;
} func_jump;

// unordered_map func_jump
typedef struct _func_jump_map {
    int f_idx;
    func_jump **fj;
} func_jump_map;

// pair<int, int>
typedef struct _ins {
    instruction ins;
    int offset;
} ins;

// vector<pair<int, int>
typedef struct _ins_list {
    int i_idx;
    ins **i;
} ins_list;

typedef struct _code_generator_handler {
    char size[VAR_SIZE_MAX];
    char *cur_space;
    const syntax_tree *tree;
    syntax_tree *node;
    const symbol_table *table;
    code_map *c_map;
    code_list *cl;
    code_list *codes;
    func_jump_map *jumps;
} code_generator_handler;

