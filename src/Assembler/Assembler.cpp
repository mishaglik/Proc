#include "Assembler.h"
#include <string.h>
#include "stdarg.h"

const char* OUT_FORMAT = ".out";
const char* LST_FORMAT = ".lst";

extern const int SIGNATURE;

const size_t PROGRAM_MIN_SZ = 1024;
const size_t EXPAND_COEF    = 2;
const size_t TO_EXPAND      = 4*sizeof(int);

const int ASM_WALKS = 0;

CompilationError assemblyFile(const char* filename){
    LOG_ASSERT(filename != NULL);

//#######################Init and reading##################################################

    CompilationError err = CompilationError::noErr;
    AsmData asmData = {};
    asmDataCtor(&asmData, filename);

    LOG_MESSAGE_F(INFO, "Beginning assemblying file %s\n", filename);
    
    FILE* inFile = fopen(filename, "r");
    if(inFile == NULL){
        LOG_MESSAGE_F(ERROR, "Error opening file: %s\n", filename);
        return;
    }
    Text programm_txt = {};
    
    LOG_ASSERT(filename != NULL);

    LOG_DEBUG("Reading text");
    readTextf(inFile, &programm_txt);
    fclose(inFile);

//##################Parsing nd Assemblying#########################################################
    LOG_DEBUG("Begin parsing text");
    parseText(&programm_txt);

    for(; asmData.nWalk < ASM_WALKS && !err; asmData.nWalk++){
        for(size_t line = 0; line < programm_txt.nStrings && !err; ++line){
            err = assemblyLine(asmData, programm_txt.strings[line].pBegin);
        }
    }

//##########################Output and freeing#####################################################
    if(!err){
        FILE* outFile = makeOutFile(filename);
        LOG_ASSERT(outFile != NULL);

        fwrite(&BIN_FILE_HEADER, sizeof(FileHeader), 1,              outFile);
        fwrite(data,             sizeof(char),       ip.instructPtr, outFile);

        fclose(outFile);
    }
    freeText(&programm_txt);
    asmDataDtor(&asmData);

    LOG_MESSAGE_F(INFO, "End assemblying file %s\n\n", filename);

    return err;
}

//------------------------------------------------------------------------------------------------------------------------

CompilationError assemblyLine(const char* data, proc_instruction_ptr_t *ip, const String* line, FILE* lstFile){
    LOG_ASSERT(line    != NULL);
    LOG_ASSERT(data    != NULL);
    LOG_ASSERT(lstFile != NULL);

    CompilationError err = CompilationError::noErr;

    //TODO: Do it better. Very crutchful;
    char* commSym = strchr(line->pBegin, ';');
    if(commSym != NULL){
        *commSym = '\0';
    }

    char* commandStr = NULL;
    sscanf(line->pBegin,"%ms", &commandStr, &argument);
    
    if(commandStr == NULL)
        return;

    proc_command_t command = {};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COM_DEF(name, value, ...)                               \
    if(strcmp(#name, commandStr) == 0) {                        \
        command = numToCom(value);                              \
    }else                                                       \
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    #include "../commands.h"
    /*else*/{
        LOG_ERROR("Compilation error::Unknown command.");
        return CompilationError::UnknownCommand;
    }
    #undef COM_DEF

    lstWrite(lstFile, "%04x %10.10s ", ip->value, commandStr);

    programm->data[ip->commandPtr++] = command;

    if(command.argMem){
        char* argStr = NULL;
        proc_arg_reg_t = 0;
        proc_arg_mem_t = 0;
        sscanf(line->pBegin, "%s [%d + ]%n")
    }

    if(hasArgument(cur_command)){
        if(programm->proc_cnt + sizeof(proc_arg_t) == programm->size)
            expandProgrammData(programm);

        *(proc_arg_t*)(programm->data + programm->proc_cnt) = argument;
        programm->proc_cnt += sizeof(proc_arg_t);
    }

    free(commandStr);
    return CompilationError::noErr;
}

//---------------------------------------------------------------------------------

void lstWrite(FILE* lstFile, const char* format, ...){
    LOG_ASSERT(lstFile != NULL);
    LOG_ASSERT(format != NULL);

    va_list args;
    va_start(args, format);
    vfprintf(LOG_FILE, format, args);
    va_end(args);
}
