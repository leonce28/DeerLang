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
    int idx, line_len, data_len;
    char line[VAR_OFFSET_MAX];
    char *data = NULL;

    if (codes == NULL || asm_file == NULL) {
        return CMM_FAILED;
    }

    data = (char *)malloc(codes->c_idx * (VAR_OFFSET_MAX * 2));
    data_len = 0;

    fp = fopen(asm_file, "w");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", asm_file);
        return CMM_FAILED;
    }

    for (idx = 0; idx < codes->c_idx; ++idx) {
        switch (codes->c[idx]->ins) {
            case INS_LDC:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "LDC %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_LD:
                strcpy(data + data_len, "LD\n");
                data_len += 3;
                break;
            case INS_ALD:
                strcpy(data + data_len, "ALD\n");
                data_len += 4;
                break;
            case INS_ST:
                strcpy(data + data_len, "ST\n");
                data_len += 3;
                break;
            case INS_AST:
                strcpy(data + data_len, "__AST\n");
                data_len += 6;
                break;
            case INS_PUSH:
                strcpy(data + data_len, "PUSH\n");
                data_len += 5;
                break;
            case INS_POP:
                strcpy(data + data_len, "POP\n");
                data_len += 4;
                break;
            case INS_JMP:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "JMP %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_JZ:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "JZ %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_ADD:
                strcpy(data + data_len, "ADD\n");
                data_len += 4;
                break;
            case INS_SUB:
                strcpy(data + data_len, "SUB\n");
                data_len += 4;
                break;
            case INS_MUL:
                strcpy(data + data_len, "MUL\n");
                data_len += 4;
                break;
            case INS_DIV:
                strcpy(data + data_len, "DIV\n");
                data_len += 4;
                break;
            case INS_LT:
                strcpy(data + data_len, "LT\n");
                data_len += 3;
                break;
            case INS_LE:
                strcpy(data + data_len, "LE\n");
                data_len += 3;
                break;
            case INS_GT:
                strcpy(data + data_len, "GT\n");
                data_len += 3;
                break;
            case INS_GE:
                strcpy(data + data_len, "GE\n");
                data_len += 3;
                break;
            case INS_EQ:
                strcpy(data + data_len, "EQ\n");
                data_len += 3;
                break;
            case INS_NE:
                strcpy(data + data_len, "NE\n");
                data_len += 3;
                break;
            case INS_IN:
                strcpy(data + data_len, "IN\n");
                data_len += 3;
                break;
            case INS_OUT:
                strcpy(data + data_len, "OUT\n");
                data_len += 4;
                break;
            case INS_ADDR:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "ADDR %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_CALL:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "CALL %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_RET:
                strcpy(data + data_len, "RET\n");
                data_len += 4;
                break;
            default:
                invalid_call("output_asm_file");
        }
    }

    fwrite(data, data_len, 1, fp);
    fclose(fp);
    free(data);
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
    syntax_tree_print(ast); 

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
