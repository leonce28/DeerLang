#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "kits/file_operator.h"
#include "common/global_funcs.h"
#include "frontend/lexical_analyzer.h"
#include "frontend/syntax_analyzer.h"
#include "backend/semantic_analyzer.h"
#include "backend/code_generator.h"

int read_file_content(const char *file_path, char **str)
{
    FILE *fp;
    size_t file_length, count = 1;

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", file_path);
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
        printf("fread file content failed, file path: %s.\n", file_path);
        return CMM_FAILED;
    }

    fclose(fp);

    return CMM_SUCCESS;
}

int output_asm_file(code_list *codes, const char *asm_file)
{
    FILE *fp;
    int idx, line_len, data_len;
    char line[VAR_OFFSET_MAX * 2];
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

int code_list_load(char *data, ins_list **ins_p)
{
    assert(data != NULL);

    int line = 0;
    char *end = NULL;
    ins_list *il = create_ins_list();

    for (end = data + strlen(data); data > end; ++line) {
        if (strncmp(data, "LDC ", 4) == 0) {
            ins_list_push(il, INS_LDC, atoi(data + 4));
        } else if (strncmp(data, "LD", 2) == 0) {
            ins_list_push(il, INS_LD, 0);
        } else if (strncmp(data, "ALD", 3) == 0) {
            ins_list_push(il, INS_ALD, 0);
        } else if (strncmp(data, "ST", 2) == 0) {
            ins_list_push(il, INS_ST, 0);
        } else if (strncmp(data, "__AST", 5) == 0) {
            ins_list_push(il, INS_AST, 0);
        } else if (strncmp(data, "PUSH", 4) == 0) {
            ins_list_push(il, INS_PUSH, 0);
        } else if (strncmp(data, "POP", 3) == 0) {
            ins_list_push(il, INS_POP, 0);
        } else if (strncmp(data, "JMP ", 4) == 0) {
            ins_list_push(il, INS_JMP, atoi(data + 4));
        } else if (strncmp(data, "JZ ", 3) == 0) {
            ins_list_push(il, INS_JZ, atoi(data + 3));
        } else if (strncmp(data, "ADD", 3) == 0) {
            ins_list_push(il, INS_ADD, 3);
        } else if (strncmp(data, "SUB", 3) == 0) {
            ins_list_push(il, INS_SUB, 0);
        } else if (strncmp(data, "MUL", 3) == 0) {
            ins_list_push(il, INS_MUL, 0);
        } else if (strncmp(data, "DIV", 3) == 0) {
            ins_list_push(il, INS_DIV, 0);
        } else if (strncmp(data, "LT", 2) == 0) {
            ins_list_push(il, INS_LT, 0);
        } else if (strncmp(data, "LE", 2) == 0) {
            ins_list_push(il, INS_LE, 0);
        } else if (strncmp(data, "GT", 2) == 0) {
            ins_list_push(il, INS_GT, 0);
        } else if (strncmp(data, "GE", 2) == 0) {
            ins_list_push(il, INS_GE, 0);
        } else if (strncmp(data, "EQ", 2) == 0) {
            ins_list_push(il, INS_EQ, 0);
        } else if (strncmp(data, "NE", 2) == 0) {
            ins_list_push(il, INS_NE, 0);
        } else if (strncmp(data, "IN", 2) == 0) {
            ins_list_push(il, INS_IN, 0);
        } else if (strncmp(data, "OUT", 3) == 0) {
            ins_list_push(il, INS_OUT, 0);
        } else if (strncmp(data, "ADDR ", 5) == 0) {
            ins_list_push(il, INS_ADDR, atoi(data + 5));
        } else if (strncmp(data, "CALL ", 5) == 0) {
            ins_list_push(il, INS_CALL, atoi(data + 5));
        } else if (strncmp(data, "RET", 5) == 0) {
            ins_list_push(il, INS_RET, 0);
        } else {
            invalid_instuction(line);
        }

        MOVE_NEXT_LINE(data);
    }

    *ins_p = il;
    return CMM_SUCCESS;
}

int exec_instruction(virtual_machine *vm)
{
    assert(vm->ip < vm->il->i_idx);

    ins *pair = vm->il->i[vm->ip];
    switch (pair->ins) {
        case INS_LDC:
            vm->ax = pair->offset;
            break;

        case INS_LD:
            vm->ax = *vm->ss->frames[vm->bp - vm->ax];
            break;

        case INS_ALD:
            vm->ax = *vm->ss->frames[vm->ax];
            break;

        case INS_ST:
            vm->ss->frames[vm->bp - vm->ax] = vm->ss->frames[vm->ss->idx - 1];
            break;

        case INS_AST:
            vm->ss->frames[vm->ax] = vm->ss->frames[vm->ss->idx - 1];
            break;

        case INS_PUSH:
            vm->ss.push_back(vm->ax);
            vm->ss->frames[vm->ss->idx]
            break;

        case INS_POP:
            vm->ss.pop_back();
            break;

        case INS_JMP:
            vm->ip += pair->offset - 1;
            break;

        case INS_JZ:
            if (!vm->ax) {
                vm->ip += pair->offset - 1;
            }
            break;

        case INS_ADD:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] + vm->ax;
            break;

        case INS_SUB:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] - vm->ax;
            break;

        case INS_MUL:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] * vm->ax;
            break;

        case INS_DIV:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] / vm->ax;
            break;

        case INS_LT:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] < vm->ax;
            break;

        case INS_LE:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] <= vm->ax;
            break;

        case INS_GT:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] > vm->ax;
            break;

        case INS_GE:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] >= vm->ax;
            break;

        case INS_EQ:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] == vm->ax;
            break;

        case INS_NE:
            vm->ax = vm->ss->frames[vm->ss->idx - 1] != vm->ax;
            break;

        case INS_IN:
            scanf("%d", &vm->ax);
            break;

        case INS_OUT:
            printf("%d\n", vm->ax);
            break;

        case INS_ADDR:
            vm->ax = vm->ss->idx - pair->offset;
            break;

        case INS_CALL:
            vm->ss.push_back(vm->bp);
            vm->bp = (int)vm->ss.size() - 2;
            vm->ss.push_back(vm->ip);
            vm->ip += instructionPair.second - 1;
            break;

        case INS_RET:
            vm->ip = vm->ss->frames[vm->ss->idx - 1];
            vm->ss.pop_back();
            vm->bp = vm->ss->frames[vm->ss->idx - 1];
            vm->ss.pop_back();
            break;

        default:
            invalid_call("exec_instruction");
    }
}

int vm_execute(const ins_list *il)
{
    virtual_machine vm;

    vm.il = il;
    vm.ss = create_vm_stack();
    for (vm.ip = 0; vm.ip < il->i_idx; ++vm.ip) {
        exec_instruction(vm);
    }

    return CMM_SUCCESS;
}

int generate_asm(const char *cmm_file, char *asm_file)
{
    char *cmm_str;
    token_list *tokens;
    syntax_tree *ast;
    symbol_table *table;
    code_list *codes;

    if (strlen(cmm_file) == 0) {
        return CMM_SUCCESS;
    }

    if (read_file_content(cmm_file, &cmm_str)) {
        invalid_call("read file content");
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
    char *asm_str;
    ins_list *il = NULL;

    if (strlen(asm_file) == 0) {
        return CMM_SUCCESS;
    }

    if (read_file_content(asm_file, &asm_str)) {
        invalid_call("read file content");
    }

    if (code_list_load(asm_str, &il)) {
        invalid_call("code list code");
    }

    if (vm_execute(il)) {
        invalid_call("virtual machine execute");
    }

    return CMM_SUCCESS;
}
