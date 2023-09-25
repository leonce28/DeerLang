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
#if 1
    printf("division by zero: %d\n",  token->line_no / (token ? 0 : 1));
#endif
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

// void init_space_symbol(symbol_space *space, const int size)
// {
//     assert(space != nullptr);

//     for (space->s_idx = 0; space->s_idx < size; ++space->s_idx) {
//         space->s[space->s_idx] = create_symbol("", 0, 0);
//     }
// }

// symbol_space *get_global_space(const symbol_table *table)
// {
//     assert(table != nullptr && (table->ss_idx > 0));
//     return table->ss[0];
// }

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

// symbol_space *get_symbol_space(symbol_table **table, const char *space_name)
// {
//     assert(table != nullptr && (*table) != nullptr && space_name != nullptr);
    
//     symbol_space *space = find_symbol_space(*table, space_name);
//     if (nullptr != space) {
//         return space;
//     }

//     (*table)->ss[(*table)->ss_idx] = create_symbol_space(space_name);

//     return (*table)->ss[(*table)->ss_idx++];
// }

SymbolTable *create_symbol_table()
{
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    memset(table, 0, sizeof(SymbolTable));
    return table;
}

// map_list *create_map_list(const char *name)
// {
//     assert(name != nullptr);

//     map_list *maps = (map_list *)malloc(sizeof(map_list));
//     memset(maps, 0, sizeof(map_list));

//     strncpy(maps->name, name, MIN(MAP_NAME_MAX, strlen(name)));
//     return maps;
// }

// code_list *create_code_list()
// {
//     code_list *cl = (code_list *)malloc(sizeof(code_list));
//     memset(cl, 0, sizeof(code_list));

//     cl->c_idx = 0;
//     cl->c = (code **)malloc(MASP_CODE_MAX * sizeof(code *));

//     return cl;
// }

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

// void set_code_map(code_map *c_map, const char *cur_space, const code_list *input_cl)
// {
//     int idx;
//     code *c;
//     c_map->maps[c_map->m_idx] = create_map_list(cur_space);
//     c_map->maps[c_map->m_idx]->cl = create_code_list();

//     for (idx = 0; idx < input_cl->c_idx; ++idx) {
//         c = create_code2(input_cl->c[idx]->ins, input_cl->c[idx]->offset);
//         c_map->maps[c_map->m_idx]->cl->c[c_map->maps[c_map->m_idx]->cl->c_idx] = c;
//         ++c_map->maps[c_map->m_idx]->cl->c_idx;
//     }

//     ++c_map->m_idx;
// }

// void code_list_clean(code_list *cl)
// {
//     int idx;

//     for (idx = 0; idx < cl->c_idx; ++idx) {
//         if (cl->c[idx]) {
//             free(cl->c[idx]);
//             cl->c[idx] = nullptr;
//         }
//     }
//     cl->c_idx = 0;
// }

code_segment *create_code_segment()
{
    code_segment *cs = (code_segment *)malloc(sizeof(code_segment));
    memset(cs, 0, sizeof(code_segment));

    cs->capacity = MASP_CODE_MAX;
    cs->size = 0;
    cs->data = (segment **)malloc(cs->capacity * sizeof(segment *));

    return cs;
}

segment *create_segment()
{
    segment *seg = (segment *)malloc(sizeof(segment));
    memset(seg, 0, sizeof(segment));

    return seg;
}

segment *create_segment2(Instruction ins, int offset)
{
    segment *seg = create_segment();

    seg->ins = ins;
    seg->offset = offset;

    return seg;
}

void code_segment_push(code_segment *cs, Instruction ins, int offset)
{
    assert(cs != nullptr);

    if (cs->size >= cs->capacity) {
        return;
    }

    cs->data[cs->size++] = create_segment2(ins, offset);
}

virtual_machine *create_virtual_machine()
{
    virtual_machine *vm = (virtual_machine *)malloc(sizeof(virtual_machine));

    memset(vm, 0, sizeof(virtual_machine));

    vm->ax = 0;
    vm->bp = 0;
    vm->ip = 0;
    vm->ss = create_vm_stack();

    return vm;
}

vm_stack *create_vm_stack()
{
    vm_stack *ss = (vm_stack *)malloc(sizeof(vm_stack));
    memset(ss, 0, sizeof(vm_stack));

    ss->capacity = MASP_CODE_MAX;
    ss->size = 0;

    return ss;
}

int vm_stack_pop(vm_stack *vm)
{
    assert(vm && vm->size >= 0);

    return vm->data[vm->size-- - 1];
}

int vm_stack_back(const vm_stack *vm)
{
    assert(vm && vm->size < vm->capacity);

    return vm->data[vm->size - 1];
}

int vm_stack_size(const vm_stack *vm)
{
    assert(vm);

    return vm->size;
}

int vm_stack_get(const vm_stack *vm, const int index)
{
    assert(vm && index < vm->size);

    return vm->data[index];
}

void vm_stack_push(vm_stack *vm, const int value)
{
    assert(vm && vm->size < vm->capacity);

    vm->data[vm->size++] = value;
}

void vm_stack_set(vm_stack *vm, const int index, const int value)
{
    assert(vm && index < vm->size);

    vm->data[index] = value;
}

int file_read_content(const char *file_path, char **str)
{
    FILE *fp;
    size_t file_length, count = 1;

    if ((fp = fopen(file_path, "r")) == nullptr) {
        printf("fopen file failed, file path: %s.\n", file_path);
        return CMM_FAILED;
    }

    fseek(fp, 0, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *str = (char *)malloc(file_length);
    if ((*str) == nullptr) {
        printf("malloc code string failed.\n");
        return CMM_FAILED;
    }

    if (count != fread((*str), file_length, count, fp)) {
        printf("fread file content failed, file path: %s.\n", file_path);
        return CMM_FAILED;
    }

    fclose(fp);
    return CMM_SUCCESS;
}

int file_write_content(const DeerLinkedList *codes, const char *asm_file)
{
    // FILE *fp;
    // int idx, line_len, data_len;
    // char line[VAR_OFFSET_MAX * 2];
    // char *data = nullptr;

    // if (codes == nullptr || asm_file == nullptr) {
    //     return CMM_FAILED;
    // }

    // if (strlen(asm_file) == 0) {
    //     asm_file = DEFAULT_ASM_PATH;
    // }

    // data = (char *)malloc(codes->c_idx * (VAR_OFFSET_MAX * 2));
    // data_len = 0;

    // fp = fopen(asm_file, "w");
    // if (fp == nullptr) {
    //     printf("fopen file failed, file path: %s.\n", asm_file);
    //     return CMM_FAILED;
    // }

    // for (idx = 0; idx < codes->c_idx; ++idx) {
    //     switch (codes->c[idx]->ins) {
    //         case INS_LDC:
    //             line_len = snprintf(line, VAR_OFFSET_MAX * 2, "LDC %s\n", codes->c[idx]->offset);
    //             strncpy(data + data_len, line, line_len);
    //             data_len += line_len;
    //             break;
    //         case INS_LD:
    //             strcpy(data + data_len, "LD\n");
    //             data_len += 3;
    //             break;
    //         case INS_ALD:
    //             strcpy(data + data_len, "ALD\n");
    //             data_len += 4;
    //             break;
    //         case INS_ST:
    //             strcpy(data + data_len, "ST\n");
    //             data_len += 3;
    //             break;
    //         case INS_AST:
    //             strcpy(data + data_len, "__AST\n");
    //             data_len += 6;
    //             break;
    //         case INS_PUSH:
    //             strcpy(data + data_len, "PUSH\n");
    //             data_len += 5;
    //             break;
    //         case INS_POP:
    //             strcpy(data + data_len, "POP\n");
    //             data_len += 4;
    //             break;
    //         case INS_JMP:
    //             line_len = snprintf(line, VAR_OFFSET_MAX * 2, "JMP %s\n", codes->c[idx]->offset);
    //             strncpy(data + data_len, line, line_len);
    //             data_len += line_len;
    //             break;
    //         case INS_JZ:
    //             line_len = snprintf(line, VAR_OFFSET_MAX * 2, "JZ %s\n", codes->c[idx]->offset);
    //             strncpy(data + data_len, line, line_len);
    //             data_len += line_len;
    //             break;
    //         case INS_ADD:
    //             strcpy(data + data_len, "ADD\n");
    //             data_len += 4;
    //             break;
    //         case INS_SUB:
    //             strcpy(data + data_len, "SUB\n");
    //             data_len += 4;
    //             break;
    //         case INS_MUL:
    //             strcpy(data + data_len, "MUL\n");
    //             data_len += 4;
    //             break;
    //         case INS_DIV:
    //             strcpy(data + data_len, "DIV\n");
    //             data_len += 4;
    //             break;
    //         case INS_LT:
    //             strcpy(data + data_len, "LT\n");
    //             data_len += 3;
    //             break;
    //         case INS_LE:
    //             strcpy(data + data_len, "LE\n");
    //             data_len += 3;
    //             break;
    //         case INS_GT:
    //             strcpy(data + data_len, "GT\n");
    //             data_len += 3;
    //             break;
    //         case INS_GE:
    //             strcpy(data + data_len, "GE\n");
    //             data_len += 3;
    //             break;
    //         case INS_EQ:
    //             strcpy(data + data_len, "EQ\n");
    //             data_len += 3;
    //             break;
    //         case INS_NE:
    //             strcpy(data + data_len, "NE\n");
    //             data_len += 3;
    //             break;
    //         case INS_IN:
    //             strcpy(data + data_len, "IN\n");
    //             data_len += 3;
    //             break;
    //         case INS_OUT:
    //             strcpy(data + data_len, "OUT\n");
    //             data_len += 4;
    //             break;
    //         case INS_ADDR:
    //             line_len = snprintf(line, VAR_OFFSET_MAX * 2, "ADDR %s\n", codes->c[idx]->offset);
    //             strncpy(data + data_len, line, line_len);
    //             data_len += line_len;
    //             break;
    //         case INS_CALL:
    //             line_len = snprintf(line, VAR_OFFSET_MAX * 2, "CALL %s\n", codes->c[idx]->offset);
    //             strncpy(data + data_len, line, line_len);
    //             data_len += line_len;
    //             break;
    //         case INS_RET:
    //             strcpy(data + data_len, "RET\n");
    //             data_len += 4;
    //             break;
    //         default:
    //             invalid_call("output_asm_file");
    //     }
    // }

    // fwrite(data, data_len, 1, fp);
    // fclose(fp);
    // free(data);
    return CMM_SUCCESS;
}