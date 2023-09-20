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

typedef struct Symbol {
    char name[VAR_NAME_MAX];
    int idx;
    int size;
} Symbol;

typedef struct SymbolSpace {
    char name[SPACE_NAME_MAX];
    DeerLinkedList *symbols;
} SymbolSpace;

typedef struct SymbolTable {
    SymbolSpace *global;
    SymbolSpace *access;
    DeerLinkedList *spaces;
} SymbolTable;

// pair<int, string>
typedef struct Code {
    Instruction ins; 
    char offset[VAR_OFFSET_MAX];
} Code;

// unordered_map<string, vector<pair<int, string>>>
typedef struct CodeMap {
    char name[MAP_NAME_MAX];
    DeerLinkedList *codes;
} CodeMap;

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
    Instruction ins;
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
    const DeerDeclList *tree;
    const DeerDeclList *table;
    DeerDeclList *node;
    // code_map *c_map;
    // code_list *cl;
    // code_list *codes;
    func_jump_map *jumps;
} CodeGenerator;

typedef struct DeerCompilerHandle {
    char cmm_file[FILE_PATH_MAX];
    char asm_file[FILE_PATH_MAX];
    char *file_content;
    DeerLinkedList *tokens;
    DeerLinkedList *codes;
    DeerLinkedList *maps;
    DeerDeclList *ast;
    DeerLexical *lex;
    SymbolTable *table;

    // symbol_table *table;
    // code_list *codes;
    code_segment *cs;
    CodeGenerator *generator;
} DeerCompilerHandle;
