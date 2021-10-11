#include "commands.h"
#include "lib/Logger.h"


#define CASE(x) case ProcCommand:: ## x: return #x
const char* strCommand(const ProcCommand command){
    switch (command)
    {
    CASE(hlt );
    CASE(push);
    CASE(pop );
    CASE(add );
    CASE(mul );
    CASE(sub );
    CASE(div );
    default:
        LOG_MESSAGE_F(FATAL, "No defined string for %d", command);
        LOG_RAISE(FATAL);
        return NULL;
    }
}
#undef CASE(x);


//-----------------------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------------------

#define CASE(x) case ProcCommand:: ## x: return x ## _func
actionFunc commandAction(const ProcCommand command){
    switch (command)
    {
    CASE(hlt );
    CASE(push);
    CASE(pop );
    CASE(add );
    CASE(mul );
    CASE(sub );
    CASE(div );
    default:
        LOG_MESSAGE_F(FATAL, "No defined string for %d", command);
        LOG_RAISE(FATAL);
        return NULL;
    }
}