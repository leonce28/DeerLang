#include <assert.h>
#include <stdlib.h>
#include "kits/generate.h"
#include "common/global_funcs.h"
#include "frontend/lexical_analyzer.h"
#include "frontend/syntax_analyzer.h"
#include "backend/semantic_analyzer.h"
#include "backend/code_generator.h"

int generate_asm(parse_handler *handler)
{
    token_list *tokens;
    syntax_tree *ast;
    symbol_table *table;
    code_list *codes;

    if (lexical_analysis(handler->cmm_path, &tokens)) {
        invild_call("lexical analysis");
    }
    // token_list_print(tokens);

    if (syntax_analysis(tokens, &ast)) {
        invild_call("syntax analysis");
    }
    // syntax_tree_print(ast);

    if (semantic_analysis(ast, &table)) {
        invild_call("semantic analysis");
    }
    // symbol_table_print(table);

    if (generate_code(ast, table, handler, &codes)) {
        invild_call("generate code");
    }

    return CMM_SUCCESS;
}
