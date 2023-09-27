#pragma once

#include <stdio.h>
#include "macro.h"

#include "common/list.h"
#include "common/tree.h"
#include "common/vector.h"

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
typedef struct FuncJump {
    char name[MAP_NAME_MAX];
    int start;
} FuncJump;

// pair<int, int>
typedef struct Segment {
    Instruction ins; 
    int offset;
} Segment;

typedef struct VirtualMachine {
    int ip;
    int ax;
    int bp;
    DeerVector *ss;
} VirtualMachine;

typedef struct DeerCompilerHandle {
    char input[FILE_PATH_MAX];
    char *content;
    const char *space;
    DeerLinkedList *tokens;
    DeerLinkedList *codes;
    DeerLinkedList *jumps;
    DeerLinkedList *maps;
    DeerDeclList *ast;
    DeerLexical *lex;
    SymbolTable *table;
    VirtualMachine *vm;
} DeerCompilerHandle;
