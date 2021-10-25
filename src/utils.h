#include <stdio.h>
#include "proc_t.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Create a out File with given extention
 * 
 * @param filename 
 * @param outputFileExt 
 * @return FILE* 
 */
FILE* createOutFile(const char* filename, const char* outputFileExt);

/**
 * @brief commnad number to proc_command_t
 * 
 * @param num 
 * @return proc_command_t 
 */
proc_command_t numToCom(const char num);

/**
 * @brief If s has no letters
 * 
 * @param s 
 * @return int 
 */
int isEndStr(const char* s);

/**
 * @brief Swap for ints
 * 
 * @param p1 
 * @param p2 
 */
void swapInt(int* p1, int* p2);

/**
 * @brief Give name of command
 * 
 * @param command 
 * @return const char* 
 */
const char* commandName(proc_command_t command);
