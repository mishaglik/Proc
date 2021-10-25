#ifndef PROC_ASSEMBLER_ASSEMBLER_H
#define PROC_ASSEMBLER_ASSEMBLER_H

#include "../config.h"
#include "../../lib/Logger.h"
#include "../../lib/text.h"
#include "../proc_t.h"
#include "../utils.h"

enum class CompilationError{
    noErr = 0,              // No error
    UnknownCommand,         // Unknown command
    MemoryError,            // Memory allocation error
    RuntimeError,           // Error on runtime
    SyntaxError,            // Syntax error
};

/**
 * @brief Label structure. Keep ip and name of label (without:)
 * 
 */
struct Label{
    proc_instruction_ptr_t ip = {};
    char name[MAX_LABEL_LEN]  = {};
};

/**
 * @brief Sturcture keeps all neccessary assembly data.
 * 
 */
struct AsmData{
    char* code  = NULL;
    size_t capacity = 0;

    proc_instruction_ptr_t ip = {0};

    int nPass = 0;

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
 * @brief asmData destructor
 * 
 * @param asmData 
 */
void asmDataDtor(AsmData* asmData);

/**
 * @brief checks if asmData->code is to be expanded
 * 
 * @param asmData 
 * @return int 
 */
int isToExpand(const AsmData* asmData);

/**
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
 * @brief Assemblyes single line
 * 
 * @param asmData 
 * @param line  - string to assmebly
 * @return CompilationError 
 */
CompilationError assemblyLine(AsmData* asmData, char* line);

/**
 * @brief Parses single argument string
 * 
 * @param asmData 
 * @param command - current command
 * @param argStr  - 
 * @param argVal 
 * @return CompilationError 
 */
CompilationError parseArgument(AsmData* asmData, proc_command_t* command, char* argStr, proc_arg_t* argVal, int nArgs);

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
 * @brief Writes data to lstFile; works only on last walkthrough and LISTENIG_FILE defined
 * 
 * @param asmData [in]
 * @param format  [in] - same as printf
 * @param ... 
 */
 void lstWrite(AsmData* asmData, const char* format, ...);

/**
 * @brief write array of bytes(data, n) to lstFile
 * 
 * @param asmData   [in]
 * @param data      [in]
 * @param n         [in]
 */
void lstWriteBytes(AsmData* asmData, const char* data, const size_t n);

/**
 * @brief Checks if s suits to label name. If so backs it's value
 * 
 * @param s         [in]
 * @param value     [out]
 * @param asmData   [in]
 * @return int 1/0 suits / no
 */
int isLabel(char* s, proc_arg_t *value, AsmData* asmData);

/**
 * @brief Checks if s suits to imm argument. If so backs it's value
 * 
 * @param s     [in]
 * @param value [out]
 * @return int 1/0 suits/0
 */
int isImmediate(const char* s, proc_arg_t* value);

/**
 * @brief Checks if s suits to reg argument. If so backs it's value
 * 
 * @param s     [in]
 * @param value [out]
 * @return int 1/0 suits/0
 */
int isRegister(const char* s, proc_arg_t* value);

CompilationError assemblyArguments(AsmData* asmData, proc_arg_t args[MAX_ARGS], int* nArg, proc_command_t* command, char** curChr);
#endif
