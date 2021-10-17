#ifndef PROC_ASSEMBLER_ASSEMBLER_H
#define PROC_ASSEMBLER_ASSEMBLER_H

#include "../../lib/Logger.h"
#include "../../lib/text.h"
#include "../Programm.h"

/**
 * @brief Assleblying file with given filename
 * 
 * @param filename - name of file to assembly
 */
void assemblyFile(const char* filename);


/**
 * @brief Assemblying one single line of code.
 * 
 * @param line - line to asm.
 */
void assemblyLine(Programm* programm, const String* line);


/**
 * @brief Create and prepares output file.
 * 
 * @param filename - input file name
 * @return FILE* - pointer to created file
 */
char* makeOutFilename(const char* filename);

#endif