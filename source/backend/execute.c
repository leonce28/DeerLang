#include "backend/execute.h"

int load_code_segment(char *data, code_segment **cs_p)
{
    assert(data != NULL);

    int line = 0;
    char *end = NULL;

    code_segment *cs = create_code_segment();
    for (end = data + strlen(data); data < end; ++line) {
        if (strncmp(data, "LDC ", 4) == 0) {
            code_segment_push(cs, INS_LDC, atoi(data + 4));
        } else if (strncmp(data, "LD", 2) == 0) {
            code_segment_push(cs, INS_LD, 0);
        } else if (strncmp(data, "ALD", 3) == 0) {
            code_segment_push(cs, INS_ALD, 0);
        } else if (strncmp(data, "ST", 2) == 0) {
            code_segment_push(cs, INS_ST, 0);
        } else if (strncmp(data, "__AST", 5) == 0) {
            code_segment_push(cs, INS_AST, 0);
        } else if (strncmp(data, "PUSH", 4) == 0) {
            code_segment_push(cs, INS_PUSH, 0);
        } else if (strncmp(data, "POP", 3) == 0) {
            code_segment_push(cs, INS_POP, 0);
        } else if (strncmp(data, "JMP ", 4) == 0) {
            code_segment_push(cs, INS_JMP, atoi(data + 4));
        } else if (strncmp(data, "JZ ", 3) == 0) {
            code_segment_push(cs, INS_JZ, atoi(data + 3));
        } else if (strncmp(data, "ADD", 3) == 0) {
            code_segment_push(cs, INS_ADD, 3);
        } else if (strncmp(data, "SUB", 3) == 0) {
            code_segment_push(cs, INS_SUB, 0);
        } else if (strncmp(data, "MUL", 3) == 0) {
            code_segment_push(cs, INS_MUL, 0);
        } else if (strncmp(data, "DIV", 3) == 0) {
            code_segment_push(cs, INS_DIV, 0);
        } else if (strncmp(data, "LT", 2) == 0) {
            code_segment_push(cs, INS_LT, 0);
        } else if (strncmp(data, "LE", 2) == 0) {
            code_segment_push(cs, INS_LE, 0);
        } else if (strncmp(data, "GT", 2) == 0) {
            code_segment_push(cs, INS_GT, 0);
        } else if (strncmp(data, "GE", 2) == 0) {
            code_segment_push(cs, INS_GE, 0);
        } else if (strncmp(data, "EQ", 2) == 0) {
            code_segment_push(cs, INS_EQ, 0);
        } else if (strncmp(data, "NE", 2) == 0) {
            code_segment_push(cs, INS_NE, 0);
        } else if (strncmp(data, "IN", 2) == 0) {
            code_segment_push(cs, INS_IN, 0);
        } else if (strncmp(data, "OUT", 3) == 0) {
            code_segment_push(cs, INS_OUT, 0);
        } else if (strncmp(data, "ADDR ", 5) == 0) {
            code_segment_push(cs, INS_ADDR, atoi(data + 5));
        } else if (strncmp(data, "CALL ", 5) == 0) {
            code_segment_push(cs, INS_CALL, atoi(data + 5));
        } else if (strncmp(data, "RET", 3) == 0) {
            code_segment_push(cs, INS_RET, 0);
        } else {
            invalid_instuction(line);
        }

        MOVE_NEXT_LINE(data);
    }

    *cs_p = cs;
    return CMM_SUCCESS;
}

static void exec_instruction(segment *seg, virtual_machine *vm)
{
    switch (seg->ins) {
        case INS_LDC:
            vm->ax = seg->offset;
            break;

        case INS_LD:
            vm->ax = vm_stack_get(vm->ss, vm->bp - vm->ax);
            break;

        case INS_ALD:
            vm->ax = vm_stack_get(vm->ss, vm->ax);
            break;

        case INS_ST:
            vm_stack_set(vm->ss, vm->bp - vm->ax, vm_stack_back(vm->ss));
            break;

        case INS_AST:
            vm_stack_set(vm->ss, vm->ax, vm_stack_back(vm->ss));
            break;

        case INS_PUSH:
            vm_stack_push(vm->ss, vm->ax);
            break;

        case INS_POP:
            vm_stack_pop(vm->ss);
            break;

        case INS_JMP:
            vm->ip += seg->offset - 1;
            break;

        case INS_JZ:
            if (!vm->ax) {
                vm->ip += seg->offset - 1;
            }
            break;

        case INS_ADD:
            vm->ax = vm_stack_back(vm->ss) + vm->ax;
            break;

        case INS_SUB:
            vm->ax = vm_stack_back(vm->ss) - vm->ax;
            break;

        case INS_MUL:
            vm->ax = vm_stack_back(vm->ss) * vm->ax;
            break;

        case INS_DIV:
            vm->ax = vm_stack_back(vm->ss) / vm->ax;
            break;

        case INS_LT:
            vm->ax = vm_stack_back(vm->ss) < vm->ax;
            break;

        case INS_LE:
            vm->ax = vm_stack_back(vm->ss) <= vm->ax;
            break;

        case INS_GT:
            vm->ax = vm_stack_back(vm->ss) > vm->ax;
            break;

        case INS_GE:
            vm->ax = vm_stack_back(vm->ss) >= vm->ax;
            break;

        case INS_EQ:
            vm->ax = vm_stack_back(vm->ss) == vm->ax;
            break;

        case INS_NE:
            vm->ax = vm_stack_back(vm->ss) != vm->ax;
            break;

        case INS_IN:
            scanf("%d", &vm->ax);
            break;

        case INS_OUT:
            printf("%d\n", vm->ax);
            break;

        case INS_ADDR:
            vm->ax = vm_stack_size(vm->ss) - seg->offset;
            break;

        case INS_CALL:
            vm_stack_push(vm->ss, vm->bp);
            vm->bp = vm_stack_size(vm->ss) - 2;
            vm_stack_push(vm->ss, vm->ip);
            vm->ip += seg->offset - 1;
            break;

        case INS_RET:
            vm->ip = vm_stack_back(vm->ss);
            vm_stack_pop(vm->ss);
            vm->bp = vm_stack_back(vm->ss);
            vm_stack_pop(vm->ss);
            break;

        default:
            printf("Invalid __Instruction value");
            exit(0);
    }
}

int vm_execute(const code_segment *cs)
{
    assert(cs != NULL);

    virtual_machine *vm = create_virtual_machine();

    for (vm->ip = 0; vm->ip < cs->size; ++vm->ip) {
        exec_instruction(cs->data[vm->ip], vm);
    }

    return CMM_SUCCESS;
}
