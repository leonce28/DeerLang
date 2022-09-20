#include "frontend/generate.h"
#include "common/global_funcs.h"
#include "frontend/lexical_analyzer.h"
#include "frontend/syntax_analyzer.h"
#include "backend/semantic_analyzer.h"

int generate_code(parse_handler *handler)
{
    int ret;
    token_list *tokens;
    syntax_tree *ast;
    // symbol_table *table;

    ret = lexical_analysis(handler->input_cmm_path, &tokens);
    if (ret != CMM_SUCCESS) {
        printf("lexical analysis failed, ret: %d\n", ret);
        return ret;
    }

    // token_list_print(tokens);

    ret = syntax_analysis(tokens, &ast);
    if (ret != CMM_SUCCESS) {
        printf("syntax analysis failed, ret: %d\n", ret);
        return ret;
    }

    syntax_tree_print(ast);

    // ret = semantic_analysis(ast, &table);
    // if (ret != CMM_SUCCESS) {
    //     printf("semantic analysis failed, ret: %d\n", ret);
    //     return ret;
    // }

    // CodeGenerator codeGenerator(root, symbolTable);

    // auto codeList = codeGenerator.generateCode();

    // IO::outputInstruction(__outputFilePath, codeList);

    return CMM_SUCCESS;
}
