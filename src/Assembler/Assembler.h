#ifndef PROC_ASSEMBLER_ASSEMBLER_H
#define PROC_ASSEMBLER_ASSEMBLER_H

#include "../../lib/Logger.h"
#include "../../lib/text.h"
#include "../proc_t.h"
#include "../utils.h"

enum class CompilationError{
    noErr = 0,
    UnknownCommand,

};

struct Label{
    proc_instruction_ptr_t ip;
    char* name;
}

struct AsmData{
    char* code  = NULL;
    size_t capacity = 0;

    proc_instruction_ptr_t ip = {0};

    int nWalk = 0;

    Label* label = NULL;

    FILE* lstFile = NULL;
}

/**
 * @brief Inits asmData with values
 * 
 * @param asmData 
 */
void asmDataCtor(const AsmData *asmData);

/**
 * 
 * @brief Expands asmData.
 * 
 * @param asmData 
 */
void expandData(const AsmData *asmData);

/**
 * @brief Assemblyes single line.
 * 
 * @param asmData current data.
 * @param line  - string to compile
 * @return CompilationError - error occuded during compilation 
 */
CompilationError assemblyLine(const AsmData* asmData, const char* line);

#endif