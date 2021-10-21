#ifndef PROC_ASSEMBLER_ASSEMBLER_H
#define PROC_ASSEMBLER_ASSEMBLER_H

#include "../../lib/Logger.h"
#include "../../lib/text.h"
#include "../proc_t.h"
#include "../utils.h"

#define MAX_LABEL_LEN 32

enum class CompilationError{
    noErr = 0,
    UnknownCommand,
    MemoryError,
    RuntimeError,
    SyntaxError,
};

struct Label{
    proc_instruction_ptr_t ip;
    char name[MAX_LABEL_LEN];
};

struct AsmData{
    char* code  = NULL;
    size_t capacity = 0;

    proc_instruction_ptr_t ip = {0};

    int nWalk = 0;

    Label* labels = NULL;
    size_t nLabels = 0;

    FILE* lstFile = NULL;
};

/**
 * @brief Inits asmData with values
 * 
 * @param asmData 
 */
void asmDataCtor(AsmData* asmData, const char* filename);

/**
 * @brief 
 * 
 * @param asmData 
 */
void asmDataDtor(AsmData* asmData);

/**
 * @brief 
 * 
 * @param asmData 
 * @return int 
 */
int isToExpand(const AsmData* asmData);

/**
 * 
 * @brief Expands asmData.
 * 
 * @param asmData 
 */
void expandData(AsmData* asmData);

/**
 * @brief Compiles file with given name. This MUST be a text file.
 * 
 * @param filename 
 * @return CompilationError 
 */
CompilationError assemblyFile(const char* filename);

/**
 * @brief Assemblyes single line.
 * 
 * @param asmData current data.
 * @param line  - string to compile
 * @return CompilationError - error occuded during compilation 
 */
CompilationError assemblyLine(AsmData* asmData, char* line);

/**
 * @brief Parces argument of command.
 * 
 * @param asmData 
 * @param command 
 * @return CompilationError 
 */
CompilationError parseArgument(const AsmData* asmData, const proc_command_t command);

/**
 * @brief Registers label with name;
 * 
 * @param asmData 
 * @param name 
 */
void registerLabel(AsmData* asmData, const char* name, proc_instruction_ptr_t ip);

/**
 * @brief Get the Label. If not found registers.
 * 
 * @param name 
 * @return proc_instruction_ptr_t 
 */
proc_instruction_ptr_t getLabel(AsmData* asmData, const char* name);

/**
 * @brief 
 * 
 * @param lstFile 
 * @param format 
 * @param ... 
 */
void lstWrite(AsmData* asmData, const char* format, ...);
#endif