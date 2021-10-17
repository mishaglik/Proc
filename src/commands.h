#ifndef PROC_COMMANDS_H
#define PROC_COMMANDS_H 
#include "stdlib.h"


#define COMMAND_APPLY(MACRO) MACRO(hlt) MACRO(push) MACRO(add) MACRO(sub) MACRO(mul) MACRO(div) MACRO(in) MACRO(out) MACRO(pop)

typedef int  proc_arg_t;
typedef char proc_com_t;


enum class ProcCommand{
    hlt   = 0x00,
    push  = 0x01,
    pop   = 0x02,
    add   = 0x03,
    sub   = 0x04,
    mul   = 0x05,
    div   = 0x06,
    out   = 0x07,
    in    = 0x08,
};

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