#pragma once
#include "common/global_struct.h"

int generate_code(const syntax_tree *ast, const symbol_table *table, parse_handler *handler, code_list **codes);