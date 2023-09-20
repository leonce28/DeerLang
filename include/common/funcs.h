#ifndef __DEER_COMMON_FUNCS_HEADER__
#define __DEER_COMMON_FUNCS_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/struct.h"

////////////////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////////////////
void invalid_call(const char *state);
void invalid_token(const DeerToken *t);
void invalid_instuction(const int line);

////////////////////////////////////////////////////////////////////////////////
// io
////////////////////////////////////////////////////////////////////////////////
int file_read_content(const char *file_path, char **str);
int file_write_content(const DeerLinkedList *codes, const char *asm_file);

////////////////////////////////////////////////////////////////////////////////
// DeerToken
////////////////////////////////////////////////////////////////////////////////
void token_print(const DeerToken *t);
DeerToken *create_token(const char *str, int type);
void token_push_char(DeerToken *t, char ch);

////////////////////////////////////////////////////////////////////////////////
// syntax_tree_list
////////////////////////////////////////////////////////////////////////////////
// DeerAST *create_ast_node(DeerToken *t);
// void syntax_tree_print(const DeerAST *ast);

////////////////////////////////////////////////////////////////////////////////
// symbol_table
////////////////////////////////////////////////////////////////////////////////
// symbol_space *get_global_space(const symbol_table *table);
// symbol_space *get_symbol_space(symbol_table **table, const char *space_name);
// symbol_space *find_symbol_space(const symbol_table *table, const char *space_name);
// Symbol *find_symbol(const symbol_table *table, const char *space_name, const char *var_name);

SymbolSpace *create_symbol_space(const char *space_name);
Symbol *create_symbol(const char *var_name, int var_idx, int var_size);
// void init_space_symbol(symbol_space *space, const int size);
void symbol_table_print(const SymbolTable *table);

Code *create_code(Instruction ins, char *offset);

////////////////////////////////////////////////////////////////////////////////
// code_list
////////////////////////////////////////////////////////////////////////////////
// code_list *create_code_list();
// int code_list_push(code_list *cl, Instruction ins, char *str);
// void code_list_push2(code_list *cl, code *c);
// void code_list_append(code_list *codes, code_list *extras);
// void code_list_print(code_list *cl);
// void code_list_set(code_list *cl, const int idx, char *offset);
// void code_list_clean(code_list *cl);

////////////////////////////////////////////////////////////////////////////////
// code_segment
////////////////////////////////////////////////////////////////////////////////
code_segment *create_code_segment();
void code_segment_push(code_segment *cs, Instruction ins, int offset);

////////////////////////////////////////////////////////////////////////////////
// map_list
////////////////////////////////////////////////////////////////////////////////
// map_list *create_map_list(const char *name);
// map_list *find_map_list(code_map *c_map, const char *space);

////////////////////////////////////////////////////////////////////////////////
// code_map
////////////////////////////////////////////////////////////////////////////////
// void set_code_map(code_map *c_map, const char *cur_space, const code_list *cl);
void code_map_print(const DeerLinkedList *maps); 
CodeMap *create_code_map(const char *name, DeerLinkedList *codes);

////////////////////////////////////////////////////////////////////////////////
// func_jump_map
////////////////////////////////////////////////////////////////////////////////
func_jump_map *create_jump_map();
int get_func_jump_num(func_jump_map *jumps, char *name);
func_jump *create_func_jump(char *func_name, int jump_num);
void set_func_jump_map(func_jump_map *jumps, char *func_name, int jump_num);
void func_jump_map_print(const func_jump_map *jumps);

////////////////////////////////////////////////////////////////////////////////
// virtual machine
////////////////////////////////////////////////////////////////////////////////
virtual_machine *create_virtual_machine();
vm_stack *create_vm_stack();
int vm_stack_pop(vm_stack *vm);
int vm_stack_back(const vm_stack *vm);
int vm_stack_size(const vm_stack *vm);
int vm_stack_get(const vm_stack *vm, const int index);
void vm_stack_push(vm_stack *vm, const int value);
void vm_stack_set(vm_stack *vm, const int index, const int value);

#endif      // __DEER_COMMON_FUNCS_HEADER__