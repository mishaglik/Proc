#include "../lib/Logger.h"
#include "../lib/text.h"

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
void assemblyLine(const String* line, const FILE* outFile);


/**
 * @brief Create and prepares output file.
 * 
 * @param filename - input file name
 * @return FILE* - pointer to created file
 */
FILE* makeOutFile(const char* filename);