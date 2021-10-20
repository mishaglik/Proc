#ifndef PROC_COMMANDS_H
#define PROC_COMMANDS_H 

typedef int  proc_arg_t;
typedef char proc_com_t;

const proc_com_t COMMAND_ARG_MASK = 0b11100000;

#define COM_DEF(name, num, ...) name = (num),

enum class ProcCommand{
    #include "commands.h"
};
#undef COM_DEF

/**
 * @brief Returns string enumeration of command
 * 
 * @param command - command to find char equivalent.
 * @return const char* 
 */
const char* strCommand(const ProcCommand command);

/**
 * @brief Checks if coommand require argument
 * 
 * @param command - command
 * @return int 
 */
int hasArgument(const ProcCommand command);
#endif