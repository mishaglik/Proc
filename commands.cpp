#include "commands.h"
#include "lib/Logger.h"

const char* outputFormat = ".out";

#define CASE(x) case ProcCommand::x : return #x;
const char* strCommand(const ProcCommand command){
    switch (command)
    {
    COMMAND_APPLY(CASE)
    default:
        LOG_MESSAGE_F(FATAL, "No defined string for %d", command);
        LOG_RAISE(FATAL);
        return NULL;
    }
}
#undef CASE


//-----------------------------------------------------------------------------------------------------------

#define DEF_FUNC(x) void x ## _func();

COMMAND_APPLY(DEF_FUNC)

void hlt_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void push_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void pop_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void add_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void sub_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void mul_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void div_func(){
    LOG_MESSAGE(INFO, "Processor halted");
    return;
}

void in_func(){
    return;
}

void out_func(){
    return;
}

//-----------------------------------------------------------------------------------------------------------

#define CASE(x) case ProcCommand::x : return x ## _func ;
actionFunc commandAction(const ProcCommand command){
    switch (command)
    {
    COMMAND_APPLY(CASE)
    default:
        LOG_MESSAGE_F(FATAL, "No defined string for %d", command);
        LOG_RAISE(FATAL);
        return NULL;
    }
}
#undef CASE
int hasArgument(const ProcCommand command){
    return command == ProcCommand::push;
}