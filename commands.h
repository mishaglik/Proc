#ifndef PROC_COMMANDS_H
#define PROC_COMMANDS_H 

#ifndef MAJOR_VERSION
#define MAJOR_VERSION 0
#endif

#ifndef MINOR_VERSION 
#define MINOR_VERSION 1
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION 0
#endif



struct FileHeader{
    int signature  = 0;
    int mj_version = 0;
    int mn_verison = 0;
    int bd_version = 0;
};

const int SIGNATURE = 'D&P';

const FileHeader FILE_HEAD = {SIGNATURE, MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION};

#define COMMAND_APPLY(MACRO) MACRO(hlt) MACRO(push) MACRO(add) MACRO(sub) MACRO(mul) MACRO(div) MACRO(in) MACRO(out) MACRO(pop)

typedef int proc_t;
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

typedef void (*actionFunc)(); 

/**
 * @brief Return process command
 * 
 * @param command 
 * @return actionFunc 
 */
actionFunc commandAction(const ProcCommand command);

/**
 * @brief Checks if coommand require argument
 * 
 * @param command - command
 * @return int 
 */
int hasArgument(const ProcCommand command);
#endif