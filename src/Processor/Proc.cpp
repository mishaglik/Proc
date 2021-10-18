#include "Proc.h"

typedef void (*commandFunc)(ProcCommand, Stack*);

void executeFile(const char* filename){
    LOG_ASSERT(filename != NULL);

    LOG_MESSAGE(INFO, "Reading programm");

    Programm* executable = freadProgramm(filename);

    Stack stack = {};
    stack_init(&stack);

    LOG_MESSAGE(INFO, "Begin executing programm");

    ProcCommand instruction = (ProcCommand)(-1);
    proc_arg_t argument = 0;

    proc_arg_t tmp1 = 0, tmp2 = 0;

    while(executable->proc_cnt != executable->size){
        instruction = (ProcCommand)executable->data[executable->proc_cnt++];    //Be careful: ++
        if(hasArgument(instruction)){
            LOG_ASSERT(executable->proc_cnt + sizeof(proc_arg_t) <= executable->size);
            
            argument = *(proc_arg_t*)(executable->data + executable->proc_cnt);
            executable->proc_cnt += sizeof(proc_arg_t);
        }

//+++++++++++++++++++++++++++++++++++++++++++++++
        #define COM_DEF(name, value, code, ...) \
            case ProcCommand::name:             \
            {code}                              \
            break;                              \
//+++++++++++++++++++++++++++++++++++++++++++++++

        switch (instruction)
        {
        #include "../commands.h"
        /*case ProcCommand::hlt:
            LOG_MESSAGE_F(INFO, "Halting\n");
            goto finish;
            break;

        case ProcCommand::push:
            stack_push(&stack, argument);
            break;

        case ProcCommand::pop:
            stack_pop(&stack);
            break;

        case ProcCommand::add:
            stack_pop(&stack, &tmp1);
            stack_pop(&stack, &tmp2);

            stack_push(&stack, tmp1 + tmp2);
            break; 

        case ProcCommand::sub:
            stack_pop(&stack, &tmp1);
            stack_pop(&stack, &tmp2);

            stack_push(&stack, tmp1 - tmp2);
            break;

        case ProcCommand::mul:
            stack_pop(&stack, &tmp1);
            stack_pop(&stack, &tmp2);

            stack_push(&stack, tmp1 * tmp2);
            break;

        case ProcCommand::div:
            stack_pop(&stack, &tmp1);
            stack_pop(&stack, &tmp2);

            stack_push(&stack, tmp1 / tmp2);
            break;

        case ProcCommand::out:
            stack_pop(&stack, &tmp1);
            printf(stack_element_format, tmp1);
            printf("\n");
            break;

        case ProcCommand::in:
            scanf(stack_element_format, &tmp1);
            stack_push(&stack,tmp1);
            break;
*/
        default:
            LOG_MESSAGE_F(FATAL,"Unknown instruction with code %d\n", (int)instruction);
            break;
        }
    #undef COM_DEF
    }
    finish:
    LOG_MESSAGE_F(INFO, "Finishing executing programm\n");

    freeProgramm(&executable);
}

