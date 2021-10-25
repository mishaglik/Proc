#ifndef PROC_DSM_DSM_H
#define PROC_DSM_DSM_H

#include "../proc_t.h"
#include "../../lib/Logger.h"
#include "../../lib/version.h"
#include "../utils.h"
#include "../../lib/File.h"

/**
 * @brief Disaseblies file
 * 
 * @param filename [in]
 */
void disassemblyFile(const char* filename);

/**
 * @brief Checks if command with name SHOULD use label
 * 
 * @param name [in]
 * @return int 
 */
int isLabelCommand(const char* name);

/**
 * @brief Pritns to out file
 * 
 * @param nWalk  [in]
 * @param file   [in]
 * @param format [in] as printf
 * @param ... 
 */
void dsmPrint(size_t nWalk, FILE* file, const char* format, ...);

/**
 * @brief Put label to array
 * 
 * @param labels    [in]
 * @param curLabel  [in]
 * @param curIp     [in]
 * @param nWalk     [in]
 */
void putLabel(proc_instruction_ptr_t labels[MAX_LABELS], size_t* curLabel, proc_instruction_ptr_t curIp, size_t nWalk);

/**
 * @brief Checks if ip should be marked as label
 * 
 * @param labels    [in]
 * @param curIp     [in]
 * @param curLabel  [in]
 * @return int 
 */
int isLabeled(proc_instruction_ptr_t labels[MAX_LABELS], proc_instruction_ptr_t curIp, size_t curLabel);
#endif
