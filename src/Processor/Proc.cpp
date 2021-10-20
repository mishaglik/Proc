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

