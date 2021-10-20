#include "utils.h"
#include "../lib/Logger.h"

const char* outputFormat = ".out";

#define COM_DEF(name,...) case ProcCommand::x : return #x;
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
#undef COM_DEF



//-----------------------------------------------------------------------------------------------------------

int hasArgument(const ProcCommand command){
    return command & COMMAND_ARG_MASK;
}