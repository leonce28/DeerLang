#pragma once

#include <stdio.h>
#include "macro.h"

#include "common/list.h"
#include "common/tree.h"

typedef struct DeerToken {
    TokenType type;
    int line_no;
    int token_len;
    char token_str[TOKEN_STR_MAX];
} DeerToken;

typedef struct DeerLexical {
    char *str;
    int line_no;
    LexicalStage stage;
} DeerLexical;

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
typedef struct _segment {
    instruction ins;
    int offset;
} segment;

// vector<pair<int, int>>
typedef struct _code_segment {
    int size;
    int capacity;
    segment **data;
} code_segment;

// vector<int>
typedef struct _vm_stack {
    int size;
    int capacity;
    int data[CODE_LSIT_MAX];
} vm_stack;

typedef struct _virtual_machine {
    int ip;
    int ax;
    int bp;
    vm_stack *ss;
} virtual_machine;

typedef struct CodeGenerator {
    char size[VAR_SIZE_MAX];
    char *cur_space;
    const DeerMultiTree *tree;
    DeerMultiTree *node;
    const symbol_table *table;
    code_map *c_map;
    code_list *cl;
    code_list *codes;
    func_jump_map *jumps;
} CodeGenerator;

typedef struct DeerCompilerHandle {
    char cmm_file[FILE_PATH_MAX];
    char asm_file[FILE_PATH_MAX];
    char *file_content;
    DeerLinkedList *tokens;
    DeerMultiTree *ast;
    DeerLexical *lex;
    symbol_table *table;
    code_list *codes;
    code_segment *cs;
    CodeGenerator *generator;
} DeerCompilerHandle;
