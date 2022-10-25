#pragma once
#include "common/global_struct.h"


////////////////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////////////////
void invalid_call(const char *state);
void invalid_token(const token *t);

////////////////////////////////////////////////////////////////////////////////
// arguments
////////////////////////////////////////////////////////////////////////////////
int arguments_init(int argc, char **argv, parse_handler *handler);

////////////////////////////////////////////////////////////////////////////////
// token
////////////////////////////////////////////////////////////////////////////////
token *create_token(const char *str, int type);
void token_print(const token_list *tokens, int token_idx);

////////////////////////////////////////////////////////////////////////////////
// token_list
////////////////////////////////////////////////////////////////////////////////
int token_list_init(token_list *tl);
int token_list_push(token_list *tl, token *t);
void token_list_print(const token_list *tl);
void token_push_char(token *t, char ch);

////////////////////////////////////////////////////////////////////////////////
// syntax_tree_list
////////////////////////////////////////////////////////////////////////////////
syntax_tree *create_ast_node(token *t);
void syntax_tree_print(const syntax_tree *ast);

////////////////////////////////////////////////////////////////////////////////
// symbol_table
////////////////////////////////////////////////////////////////////////////////
symbol_table *create_symbol_table();
symbol_space *create_symbol_space(const char *space_name);
symbol_space *get_global_space(const symbol_table *table);
symbol_space *get_symbol_space(symbol_table **table, const char *space_name);
symbol_space *find_symbol_space(const symbol_table *table, const char *space_name);
symbol *create_symbol(const char *var_name, int var_idx, int var_size);
symbol *find_symbol(const symbol_table *table, const char *space_name, const char *var_name);
void symbol_table_print(const symbol_table *table);

////////////////////////////////////////////////////////////////////////////////
// code_list
////////////////////////////////////////////////////////////////////////////////
code_list *create_code_list();
void code_list_push(code_list *cl, instruction ins, char *str);
void code_list_push2(code_list *cl, code *c);
void code_list_append(code_list *codes, code_list *extras);

////////////////////////////////////////////////////////////////////////////////
// map_list
////////////////////////////////////////////////////////////////////////////////
map_list *create_map_list(const char *name);
map_list *find_map_list(code_map *c_map, const char *space);

////////////////////////////////////////////////////////////////////////////////
// code_map
////////////////////////////////////////////////////////////////////////////////
code_generator_handler *get_code_generator_handler(syntax_tree *tree, symbol_table *table);
void set_code_map(code_map *c_map, const char *cur_space, code_list *cl);

////////////////////////////////////////////////////////////////////////////////
// func_jump_map
////////////////////////////////////////////////////////////////////////////////
func_jump_map *create_jump_map();
int get_func_jump_num(func_jump_map *jumps, char *name);
func_jump *create_func_jump(char *func_name, int jump_num);
void set_func_jump_map(func_jump_map *jumps, char *func_name, int jump_num);