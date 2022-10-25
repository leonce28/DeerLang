#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "kits/file_operator.h"
#include "common/global_funcs.h"
#include "frontend/lexical_analyzer.h"
#include "frontend/syntax_analyzer.h"
#include "backend/semantic_analyzer.h"
#include "backend/code_generator.h"

int read_code_string(const char *cmm_path, char **str)
{
    FILE *fp;
    size_t file_length, count = 1;

    fp = fopen(cmm_path, "r");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", cmm_path);
        return CMM_FAILED;
    }

    fseek(fp, 0, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *str = (char *)malloc(file_length);
    if ((*str) == NULL) {
        printf("malloc code string failed.\n");
        return CMM_FAILED;
    }

    if (count != fread((*str), file_length, count, fp)) {
        printf("fread cmm file failed, file path: %s.\n", cmm_path);
        return CMM_FAILED;
    }

    fclose(fp);

    return CMM_SUCCESS;
}

int output_asm_file(code_list *codes, const char *asm_file)
{
    FILE *fp;
    int idx;
    char line[VAR_OFFSET_MAX * 2];

    if (codes == NULL || asm_file == NULL) {
        return CMM_FAILED;
    }

    fp = fopen(asm_file, "w");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", asm_file);
        return CMM_FAILED;
    }

    for (idx = 0; idx < codes->c_idx; ++idx) {
        memset(line, 0, VAR_OFFSET_MAX * 2);
        snprintf(line, VAR_OFFSET_MAX * 2, "%d %s\n", codes->c[idx]->ins, codes->c[idx]->offset);
        fwrite(line, sizeof(line), 1, fp);
    }

    fclose(fp);
    return CMM_SUCCESS;
}

int generate_asm(const char *cmm_file, char *asm_file)
{
    char *cmm_str;
    token_list *tokens;
    syntax_tree *ast;
    symbol_table *table;
    code_list *codes;

    if (read_code_string(cmm_file, &cmm_str)) {
        invalid_call("read code string");
    }

    if (lexical_analysis(cmm_str, &tokens)) {
        invalid_call("lexical analysis");
    }
    // token_list_print(tokens);

    if (syntax_analysis(tokens, &ast)) {
        invalid_call("syntax analysis");
    }
    // syntax_tree_print(ast);

    if (semantic_analysis(ast, &table)) {
        invalid_call("semantic analysis");
    }
    // symbol_table_print(table);

    if (generate_code(ast, table, &codes)) {
        invalid_call("generate code");
    }
    // code_list_print(codes);

    if (output_asm_file(codes, asm_file)) {
        invalid_call("output asm file");
    }

    return CMM_SUCCESS;
}

int execute_code(const char *asm_file)
{
    return CMM_SUCCESS;
}
