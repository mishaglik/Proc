#ifndef PROC_PROC_H
#define PROC_PROC_H

#include "../../lib/Logger.h"
#include "../../lib/Stack.h"
#include "../../lib/version.h"
#include "../../lib/File.h"
#include "../proc_t.h"
#include "../config.h"
#include "../utils.h"

#ifdef VIDEO
#include "VideoDriver.h"
#endif

enum class RuntimeError{
    noErr,              //No error
    MemoryAccessErr,    //Memory access error
    UnknownCommand,     //Unknown command
    EmptyStack,         //Pop from empty stack
    StackOverflow,      //Stack is too large
    TooManyOperations,  //Too many operations. E.g. inf loop;
};

enum class ProcStatus{
    OFF,
    Idle,
    Running,
    Halted,
    Error,
};

struct RAM{
    char* data;
};

/**
 * @brief Inits ram
 * 
 * @param ram 
 * @return int if init successfull 
 */
int RAM_init(RAM* ram);

struct Processor{
    Stack stack             = {};
    RAM ram                 = {};
    char* code              = NULL;
    proc_arg_t reg[NREGS]   = {};


#ifdef VIDEO
    VideoDriver videoDriver = {};
#endif

    ProcStatus status = ProcStatus::OFF;
    proc_instruction_ptr_t ip = {};
};

/**
 * @brief Get request to memory and backs pointer to requested value.
 * Should be slooooow;
 * 
 * @param proc      [in]
 * @param address   [in]
 * @param arg       [out]
 * @return RuntimeError 
 */
RuntimeError RAM_getPtr(Processor* proc, proc_arg_t address, proc_arg_t** arg);

/**
 * @brief Loads processor with code from file with given name
 * 
 * @param proc      [in]
 * @param filename  [out]
 */
int processorLoad(Processor* proc, const char* filename );


/**
 * @brief Start executing processor's code.
 * 
 * @param proc [in]
 * @return RuntimeError
 */
RuntimeError processorExecute(Processor* proc);

/**
 * @brief Frees processor
 * 
 * @param proc  [in]
 */
void processorFree(Processor* proc);

/**
 * @brief Get the Arg object (proc_arg_t*)
 * 
 * @param proc      [in]
 * @param arg       [out]
 * @param command   [in] - current command
 * @param immArg    [out]- place for immArg
 * @return RuntimeError 
 */
RuntimeError getArg(Processor* proc, proc_arg_t** arg, proc_command_t command, proc_arg_t* immArg);

/**
 * @brief Dumps processors info;
 * 
 * @param proc [in]
 */
void procDump(Processor* proc);

/**
 * @brief Dump n bytes from data. Colors (toColor)'s byte
 * 
 * @param data      [in]
 * @param n         [in]
 * @param toColor   [in]
 */
void dumpBytes(char* data, size_t n, size_t toColor = 0);

/**
 * @brief Dumps stack
 * 
 * @param stack [in]
 */
void dumpStack(Stack* stack);

/**
 * @brief Dump Memory of proc_arg_t
 * 
 * @param data [in]
 * @param n    [in]
 */
void dumpMem(proc_arg_t* data, size_t n);

#endif
