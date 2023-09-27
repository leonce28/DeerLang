#include "common/funcs.h"

void invalid_call(const char *state)
{
    assert(state);
    printf("%s failed\n", state);
    exit(0);
}

void invalid_node(const DeerNode *n)
{
    printf("invalid node: %d\n", n->type);
}

void invalid_token(const DeerToken *token)
{
    printf("invalid token: '%s' in line %d\n", token->token_str, token->line_no);
}

void invalid_instuction(const int line)
{
    printf("invalid instuction in line %d\n", line);
    exit(0);
}

DeerToken *create_token(const char *str, int type)
{
    DeerToken *token = (DeerToken *)malloc(sizeof(DeerToken));
    if (!token) {
        return nullptr;
    }

    memset(token, 0, sizeof(DeerToken));
    token->type = type;
    token->token_len = 0;
    token->line_no = 0;

    if (str != nullptr) {
        token->token_len = MIN(TOKEN_STR_MAX, strlen(str));
        strncpy(token->token_str, str, token->token_len);
    }
    return token;
}

void token_push_char(DeerToken *token, char ch)
{
    assert(token);

    if (token->token_len >= TOKEN_STR_MAX) {
        printf("DeerToken length is too long for token_str array, max length: %d.\n", TOKEN_STR_MAX);
        exit(0);
    }

    token->token_str[token->token_len++] = ch;
}

Symbol *create_symbol(const char *name, int idx, int size)
{
    assert(name);

    Symbol *s = (Symbol *)malloc(sizeof(Symbol));
    memset(s, 0, sizeof(Symbol));
    strncpy(s->name, name, MIN(VAR_NAME_MAX, strlen(name)));
    s->idx = idx;
    s->size = size;

    return s;
}

const Symbol *find_symbol(const SymbolTable *table, const char *name, const char *id)
{
    assert(table && name && id);

    const Symbol *symbol = nullptr;
    const SymbolSpace *space = nullptr;
    
    if ((space = find_symbol_space(table, name)) == nullptr) {
        return nullptr;
    }

    foreach (Symbol, symbol, space->symbols) {
        if (strcmp(id, symbol->name) == 0) {
            return symbol;
        }
    }

    return nullptr;
}

const Symbol *find_symbol_global(const SymbolTable *table, const char *id)
{
    assert(table && id);

    const Symbol *symbol = nullptr;
    
    if (table->global == nullptr) {
        return nullptr;
    }

    foreach (Symbol, symbol, table->global->symbols) {
        if (strcmp(id, symbol->name) == 0) {
            return symbol;
        }
    }

    return nullptr;
}

SymbolSpace *create_symbol_space(const char* space_name)
{
    assert(space_name);

    SymbolSpace *space = (SymbolSpace *)malloc(sizeof(SymbolSpace));
    assert(space);
    memset(space, 0, sizeof(SymbolSpace));
    strncpy(space->name, space_name, MIN(SPACE_NAME_MAX, strlen(space_name)));

    return space;
}

// exclude global namespace
const SymbolSpace *find_symbol_space(const SymbolTable *table, const char *name)
{
    assert(table && name);
    const SymbolSpace *space = nullptr;

    // main function
    if (strcmp(name, NAMESPACE_ACCESS) == 0) {
        return table->access;
    } 
    
    foreach (SymbolSpace, space, table->spaces) {
        if (strcmp(name, space->name) == 0) {
            return space;
        } 
    }
  
    return nullptr;
}

SymbolTable *create_symbol_table()
{
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    memset(table, 0, sizeof(SymbolTable));
    return table;
}

Code *create_code(Instruction ins, char *offset)
{
    Code *code = (Code *)malloc(sizeof(Code));
    memset(code, 0, sizeof(Code));

    code->ins = ins;

    if (offset) {
        strncpy(code->offset, offset, VAR_OFFSET_MAX);
    }

    return code;
}

FuncJump *create_func_jump(char *name, int start)
{
    assert(name);

    FuncJump *jump = (FuncJump *)malloc(sizeof(FuncJump));
    memset(jump, 0, sizeof(FuncJump));

    strncpy(jump->name, name, MAP_NAME_MAX);
    jump->start = start;

    return jump;
}

FuncJump *find_func_jump(DeerLinkedList *jumps, const char *name)
{
    if (!jumps || !name) {
        return nullptr;
    }

    FuncJump *jump = nullptr;

    foreach (FuncJump, jump, jumps) {
        if (strcmp(jump->name, name) == 0) {
            return jump;
        }
    }

    return nullptr;
}

CodeMap *create_code_map(const char *name, DeerLinkedList *codes)
{
    CodeMap *map = (CodeMap *)malloc(sizeof(CodeMap));

    memset(map, 0, sizeof(CodeMap));

    strncpy(map->name, name, MIN(MAP_NAME_MAX, strlen(name)));
    map->codes = codes;

    return map;
} 

CodeMap *find_code_map(DeerLinkedList *maps, const char *name)
{
    if (!maps || !name) {
        return nullptr;
    }

    CodeMap *map = nullptr;

    foreach (CodeMap, map, maps) {
        if (strcmp(map->name, name) == 0) {
            return map;
        }
    }

    return nullptr;
}