#include <stdio.h>
#include <stdlib.h>
#include "front/lexical_analysis.h"
#include "common/global_macro.h"

int read_code_string(const char *cmm_file_path, char **code_str)
{
    FILE *fp;
    size_t file_length, count = 1;

    fp = fopen(cmm_file_path, "r");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", cmm_file_path);
        return CMM_FAILED;
    }

    fseek(fp, 0, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *code_str = (char *)malloc(file_length);
    if ((*code_str) == NULL) {
        printf("malloc code string failed.\n");
        return CMM_FAILED;
    }

    if (count != fread((*code_str), file_length, count, fp)) {
        printf("fread cmm file failed, file path: %s.\n", cmm_file_path);
        return CMM_FAILED;
    }

    fclose(fp);

    return CMM_SUCCESS;
}

int lexical_analysis(const char *cmm_file_path, token_list *tokens)
{
    char *code_str;
    if (CMM_SUCCESS != read_code_string(cmm_file_path, &code_str) || code_str == NULL) {
        printf("lexical analysis read code string failed. \n");
        return CMM_FAILED;
    }

    return CMM_SUCCESS;
}