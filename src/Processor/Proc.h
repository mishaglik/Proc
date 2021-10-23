#ifndef PROC_PROC_H
#define PROC_PROC_H

#include "../../lib/Logger.h"
#include "../../lib/Stack.h"
#include "../../lib/version.h"
#include "../../lib/File.h"
#include "../proc_t.h"
#include "../config.h"
#include "VideoDriver.h"


enum class RuntimeError{
    noErr,
    MemoryAccessErr,
    UnknownCommand,
    EmptyStack,

};

enum class ProcStatus{
    OFF,
    Idle,
    Running,
    Halted,
    Error,
};

struct RAM{
    proc_arg_t data[RAM_SZ];
};


struct Processor{
    Stack stack = {};
    char* code  = NULL;
    proc_arg_t reg[NREGS] = {};
    RAM ram = {}; 
    VideoDriver videoDriver = {};
    
    ProcStatus status = ProcStatus::OFF;
    proc_instruction_ptr_t ip = {0};
};

RuntimeError RAM_getPtr(Processor* proc, proc_arg_t address, proc_arg_t** arg);

/**
 * @brief 
 * 
 * @param proc 
 * @param filename 
 */
int processorLoad(Processor* proc, const char* filename );


/**
 * @brief 
 * 
 * @param proc 
 */
RuntimeError processorExecute(Processor* proc);

/**
 * @brief 
 * 
 * @param proc 
 */
void processorFree(Processor* proc);

/**
 * @brief Get the Arg object
 * 
 * @param proc 
 * @param arg 
 * @param command 
 * @param immArg 
 * @return RuntimeError 
 */
RuntimeError getArg(Processor* proc, proc_arg_t** arg, proc_command_t command, proc_arg_t* immArg);

void procDump(Processor* proc);

#endif