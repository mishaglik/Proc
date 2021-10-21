#ifndef PROC_PROC_H
#define PROC_PROC_H

#include "../../lib/Logger.h"
#include "../../lib/Stack.h"
#include "../../lib/version.h"
#include "../../lib/File.h"
#include "../proc_t.h"

#define RAM_SZ 1024

enum class RuntimeError{
    noErr,
    MemoryAccessErr,
    UnknownCommand,

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

RuntimeError RAM_getPtr(RAM* ram, proc_arg_t address, proc_arg_t** arg);

struct Processor{
    Stack stack = {};
    char* code  = NULL;
    proc_arg_t reg[NREGS] = {};
    RAM ram = {}; 
    
    ProcStatus status = ProcStatus::OFF;
    proc_instruction_ptr_t ip = {0};
};

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


RuntimeError getArg(Processor* proc, proc_arg_t** arg, proc_command_t command, proc_arg_t* immArg);

#endif