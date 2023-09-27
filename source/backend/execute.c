#include "backend/execute.h"

void execute_code(Code *code, VirtualMachine *vm)
{
    int offset = 0;
    Segment *segment = nullptr;

    switch (code->ins) {
        case INS_LDC:
            vm->ax = atoi(code->offset);
            break;

        case INS_LD:
            segment = dvector_fetch(vm->ss, vm->bp - vm->ax);
            vm->ax = segment->offset;
            break;

        case INS_ALD:
            segment = dvector_fetch(vm->ss, vm->ax);
            vm->ax = segment->offset;
            break;

        case INS_ST:
            segment = dvector_back(vm->ss);
            offset = segment->offset;

            segment = dvector_fetch(vm->ss, vm->bp - vm->ax);
            segment->offset = offset;
            break;

        case INS_AST:
            segment = dvector_back(vm->ss);
            offset = segment->offset;

            segment = dvector_fetch(vm->ss, vm->ax);
            segment->offset = offset;
            break;

        case INS_PUSH:
            segment = dvector_fetch_back(vm->ss);
            segment->offset = vm->ax;
            break;

        case INS_POP:
            dvector_pop(vm->ss);
            break;

        case INS_JMP:
            vm->ip += atoi(code->offset) - 1;
            break;

        case INS_JZ:
            if (!vm->ax) {
                vm->ip += atoi(code->offset) - 1;
            }
            break;

        case INS_ADD:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset + vm->ax;
            break;

        case INS_SUB:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset - vm->ax;
            break;

        case INS_MUL:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset * vm->ax;
            break;

        case INS_DIV:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset / vm->ax;
            break;

        case INS_LT:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset < vm->ax;
            break;

        case INS_LE:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset <= vm->ax;
            break;

        case INS_GT:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset > vm->ax;
            break;

        case INS_GE:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset >= vm->ax;
            break;

        case INS_EQ:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset == vm->ax;
            break;

        case INS_NE:
            segment = dvector_back(vm->ss);
            vm->ax = segment->offset != vm->ax;
            break;

        case INS_IN:
            scanf("%d", &vm->ax);
            break;

        case INS_OUT:
            printf("%d\n", vm->ax);
            break;

        case INS_ADDR:
            vm->ax = vm->ss->size - atoi(code->offset);
            break;

        case INS_CALL:
            segment = dvector_fetch_back(vm->ss);
            segment->offset = vm->bp;

            vm->bp = vm->ss->size - 2;

            segment = dvector_fetch_back(vm->ss);
            segment->offset = vm->ip;

            vm->ip += atoi(code->offset) - 1;
            break;

        case INS_RET:
            segment = dvector_back(vm->ss);
            vm->ip = segment->offset;
            dvector_pop(vm->ss);
        
            segment = dvector_back(vm->ss);
            vm->bp = segment->offset;
            dvector_pop(vm->ss);
            break;

        default:
            printf("Invalid __Instruction value");
            exit(0);
    }
}
