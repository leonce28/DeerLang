#include "front/generate.h"
#include "front/lexical_analysis.h"
#include "front/syntax_analyzer.h"
#include "common/global_instance.h"

int generate_code(parse_handler *handler)
{
    int ret;
    token_list *tokens;
    syntax_tree *ast;

    ret = get_token_list_instance(&tokens);
    if (ret != CMM_SUCCESS) {
        printf("get token list instance failed, ret: %d\n", ret);
        return ret;
    }

    ret = lexical_analysis(handler->input_cmm_path, tokens);
    if (ret != CMM_SUCCESS) {
        printf("lexical analysis failed, ret: %d\n", ret);
        return ret;
    }

    // tokens->op->print((void *)tokens);

    ret = syntax_analysis(tokens, &ast);
    if (ret != CMM_SUCCESS) {
        printf("syntax analysis failed, ret: %d\n", ret);
        return ret;
    }

    // SemanticAnalyzer semanticAnalyzer(ast);

    // auto symbolTable = semanticAnalyzer.semanticAnalysis();

    // CodeGenerator codeGenerator(root, symbolTable);

    // auto codeList = codeGenerator.generateCode();

    // IO::outputInstruction(__outputFilePath, codeList);

    return CMM_SUCCESS;
}
