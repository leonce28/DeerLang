#pragma once
#include "common/global_struct.h"


////////////////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////////////////
void invild_call(const char *state);

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
symbol *create_symbol(const char *var_name, int var_idx, int var_size);
void symbol_table_print(const symbol_table *table);

////////////////////////////////////////////////////////////////////////////////
// code_list
////////////////////////////////////////////////////////////////////////////////
// code_list *create_code_list();

////////////////////////////////////////////////////////////////////////////////
// code_map
////////////////////////////////////////////////////////////////////////////////
unordered_code_map *create_unordered_code_map();
