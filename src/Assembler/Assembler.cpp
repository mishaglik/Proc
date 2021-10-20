#include "Assembler.h"
#include <string.h>
#include "stdarg.h"

const char* OUT_FORMAT = ".out";
const char* LST_FORMAT = ".lst";

extern const int SIGNATURE;

const size_t PROGRAM_MIN_SZ = 1024;
const size_t EXPAND_COEF    = 2;
const size_t TO_EXPAND      = 4*sizeof(int);
const size_t MAX_LABELS     = 1024;

const int ASM_WALKS = 1;

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
        return CompilationError::RuntimeError;
    }
    Text programm_txt = {};
    
    LOG_ASSERT(filename != NULL);

    LOG_MESSAGE_F(INFO,"Reading text\n");
    readTextf(inFile, &programm_txt);
    fclose(inFile);

//##################Parsing nd Assemblying#########################################################
    LOG_MESSAGE_F(INFO,"Begin parsing text\n");
    parseText(&programm_txt);
    
    LOG_MESSAGE_F(INFO,"Begin assemblying text\n");
    for(; asmData.nWalk < ASM_WALKS && err == CompilationError::noErr; asmData.nWalk++){
        LOG_MESSAGE_F(INFO, "Walkthrough %d\n", asmData.nWalk);
        for(size_t line = 0; line < programm_txt.nStrings && err == CompilationError::noErr; ++line){
            LOG_MESSAGE_F(INFO, "Line %d:\n", line);

            err = assemblyLine(&asmData, programm_txt.strings[line].pBegin);
        }
    }

//##########################Output and freeing#####################################################
    LOG_MESSAGE_F(INFO,"Assemblying finished with err = %d. out & free\n", err);
    if(err == CompilationError::noErr){
        FILE* outFile = createOutFile(filename, OUT_FORMAT);
        LOG_ASSERT(outFile != NULL);

        fwrite(&BIN_FILE_HEADER, sizeof(FileHeader), 1,                outFile);
        fwrite(asmData.code,     sizeof(char),       asmData.ip.value, outFile);

        fclose(outFile);
    }
    freeText(&programm_txt);
    asmDataDtor(&asmData);

    LOG_MESSAGE_F(INFO, "End assemblying file %s\n\n", filename);

    return err;
}

//------------------------------------------------------------------------------------------------------------------------

CompilationError assemblyLine(AsmData* asmData, char* line){
    LOG_ASSERT(asmData != NULL);
    LOG_ASSERT(line    != NULL);

    CompilationError err = CompilationError::noErr;


    //TODO: Do it better. Very crutchful; Comment ignoring
    char* commentSym = strchr(line, ';');
    if(commentSym != NULL){
        *commentSym = '\0';
    }

    char* commandStr = NULL;
    size_t commSz = 0;
    sscanf(line, "%m[A-Za-z]%n", &commandStr, &commSz);
    
    if(commandStr == NULL || commandStr == ""){
        LOG_MESSAGE_F(INFO, "Skipped\n");
        sscanf(line, "%*c%n", &commSz);
        return commSz ? CompilationError::SyntaxError : CompilationError::noErr;
    }
    proc_command_t command = {};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COM_DEF(name, val, ...)                                 \
    if(strcmp(#name, commandStr) == 0) {                        \
        command.value = val;                                    \
    }else                                                       \
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    #include "../commands.h"
    /*else*/{
        char nxtChr = 0;
        sscanf(line + commSz, "%c", &nxtChr);
        if(nxtChr == ':'){
            registerLabel(asmData, commandStr);
            lstWrite(asmData->lstFile,"%s:\n", commandStr);
            return CompilationError::noErr;
        }
        else{
            LOG_ERROR("Compilation error::Unknown command.");
            return CompilationError::UnknownCommand;
        }
    }
    #undef COM_DEF
    lstWrite(asmData->lstFile, "%04x %-15.15s ", asmData->ip.value, line);
    // LOG_MESSAGE_F(INFO, "Begin parsing argument\n");git
    if(command.flags.argMem){
        char* argStr = NULL;
        sscanf(line + commSz, " [ %m[abcdx0123456789+ ] ]", &argStr);

        if(argStr == NULL){
            command.flags.argMem = 0;
            argStr = line + commSz;
        }
        proc_arg_t immArg = 0;
        proc_arg_t regArg = 0;
        char regName[2] = {};
        size_t nRead = 0;

        if(sscanf(argStr, "%1[abcd]x + %d", regName, &immArg) == 2 || sscanf(argStr, " %d + %1[abcd]x", &immArg, regName) == 2); 
        else if(sscanf(argStr, "%d", &immArg)){
            command.flags.argReg = 0;
        }
        else if(sscanf(argStr, " %1[abcd]x%n ", regName, &nRead)){
            command.flags.argImm = 0;
        }
        else{
            err = CompilationError::SyntaxError;
        }
        if(argStr != line + commSz)
            free(argStr);  
        if(err == CompilationError::noErr){
            lstWrite(asmData->lstFile, "%02X ", command.value);
            asmData->code[asmData->ip.value] = command.value;
            asmData->ip.commandPtr++;

            if(command.flags.argImm){
                lstWrite(asmData->lstFile, "%08X ", immArg);
                asmData->code[asmData->ip.value] = immArg;
                asmData->ip.argPtr++;
            }

            if(command.flags.argReg){
                regArg = regName[0] - 'a' + 1;
                lstWrite(asmData->lstFile, "%08X ", regArg);
                asmData->code[asmData->ip.value] = regArg;
                asmData->ip.argPtr++;
            }

            lstWrite(asmData->lstFile, "\n");
        }
            
        
    }
    else{
        lstWrite(asmData->lstFile, "%02X ", command.value);
        asmData->code[asmData->ip.value] = command.value;
        asmData->ip.commandPtr++;

    }
    lstWrite(asmData->lstFile, "\n");
    free(commandStr);
    return err;
}

//---------------------------------------------------------------------------------

void lstWrite(FILE* lstFile, const char* format, ...){
    LOG_ASSERT(lstFile != NULL);
    LOG_ASSERT(format != NULL);

    va_list args;
    va_start(args, format);
    vfprintf(lstFile, format, args);
    va_end(args);
}

void asmDataCtor(AsmData* asmData, const char* filename){
    LOG_ASSERT(asmData  != NULL);
    LOG_ASSERT(filename != NULL);

    asmData->code = (char*)calloc(PROGRAM_MIN_SZ, sizeof(char));
    asmData->capacity = PROGRAM_MIN_SZ;

    LOG_ASSERT(asmData->code != NULL);

    asmData->labels = (Label*)calloc(MAX_LABELS, sizeof(Label));

    asmData->lstFile = createOutFile(filename, LST_FORMAT);

    asmData->ip.value = 0;
    asmData->nWalk = 0;
}

void asmDataDtor(AsmData* asmData){
    free(asmData->code);
    free(asmData->labels);
    if(asmData->lstFile)
        fclose(asmData->lstFile);
}

int isToExpand(const AsmData* asmData){
    LOG_ASSERT(asmData != NULL);
    return (asmData->capacity - asmData->ip.value <= TO_EXPAND);
}

void expandData(AsmData* asmData){
    LOG_ASSERT(asmData != NULL);

    char* newPtr = (char*)realloc(asmData->code, (asmData->capacity *= EXPAND_COEF) * sizeof(char));
    LOG_ASSERT(newPtr != NULL);

    asmData->code = newPtr;
}

void registerLabel(AsmData* asmData, const char* name){
    return;
}

proc_instruction_ptr_t getLabel(const char* name){
    return {-1};
}