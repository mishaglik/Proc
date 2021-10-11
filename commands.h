#ifndef PROC_COMMANDS_H
#define PROC_COMMANDS_H 

#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#ifndef BUILD_VERSION
#define BUILD_VERSION 0
#endif
#define SIGNATYRE 'DaP'

enum class ProcCommand{
    hlt   = 0,
    push  = 1,
    pop   = 2,
    add   = 3,
    sub   = 4,
    mul   = 5,
    div   = 6,
    out   = 7,
    in    = 8,
};

const char* outputFormat = ".out";



/**
 * @brief Returns string enumeration of command
 * 
 * @param command - command to find char equivalent.
 * @return const char* 
 */
const char* strCommand(const ProcCommand command);

typedef void (*actionFunc)(); 

/**
 * @brief Return process command
 * 
 * @param command 
 * @return actionFunc 
 */
actionFunc commandAction(const ProcCommand command);


#endif