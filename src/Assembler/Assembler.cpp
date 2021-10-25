#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "Assembler.h"


#define TOKEN "32[A-Za-z0-9:_-]"

const proc_instruction_ptr_t UNDEF_PT = {-1};

const int ASM_PASSES = 2;

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
    for(; asmData.nPass < ASM_PASSES && err == CompilationError::noErr; asmData.nPass++){
        LOG_MESSAGE_F(INFO, "Walkthrough %d\n", asmData.nPass);
        asmData.ip.value = 0;
        for(size_t line = 0; line < programm_txt.nStrings; ++line){
            LOG_MESSAGE_F(INFO, "Line %d:\n", line);

            err = assemblyLine(&asmData, programm_txt.strings[line].pBegin);
            if(err != CompilationError::noErr){
                LOG_MESSAGE_F(ERROR, "Compilation error on line: %d\n", line);
                break;
            }
            if(isToExpand(&asmData))
                expandData(&asmData);
        }
    }
    LOG_MESSAGE_F(INFO,"Assemblying finished with error code = %d\n", err);

//##########################Output and freeing#####################################################
    LOG_MESSAGE_F(INFO, "Out & free\n");
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

    char* commentSym = strchr(line, ';');
    if(commentSym != NULL){
        *commentSym = '\0';
    }

    char  token[MAX_TOKEN_LEN]  = {};
    char* curChr = line;
    int   nRead  = 0;

    sscanf(curChr, " %"TOKEN"%n", token, &nRead);
    curChr += nRead;

    LOG_DEBUG_F("Token: \"%s\"\n", token);

    if(token[0] == '\0'){
        LOG_MESSAGE_F(INFO, "Skipped: %s\n", line);
        return isEndStr(line) ? CompilationError::noErr : CompilationError::SyntaxError;
    }

//########################## Directives #######################################
    if(isLabel(token, NULL, asmData)){
        LOG_MESSAGE_F(INFO, "Label: %s\n", token);
        
        registerLabel(asmData, token, asmData->ip);
        lstWrite(asmData, "%04x %s\n", asmData->ip.value, token);
        
        return isEndStr(curChr) ? CompilationError::noErr : CompilationError::SyntaxError;
    }

    if(strcmp(token, "db") == 0){
        char* str = NULL;
        if(sscanf(curChr, " \" %m[^\n\"] \"%n", &str, &nRead) != 1){
            return CompilationError::SyntaxError;
        }
        LOG_DEBUG_F("String: \"%s\"\n", str);
        curChr += nRead;
        size_t len = strlen(str);

        memcpy(asmData->code + asmData->ip.value, str, len + 1);
        asmData->ip.value += len + 1;

        free(str);
        return isEndStr(curChr) ? CompilationError::noErr : CompilationError::SyntaxError;
    }

//########################## Commands   #######################################
    proc_command_t command = {};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COM_DEF(name, val, ...)                                 \
    if(strcmp(#name, token) == 0) {                             \
        command.value = (char)val;                              \
    }else                                                       \
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    #include "../commands.h"

    /*else*/{
        LOG_MESSAGE_F(ERROR, "Error: Unknown command: %s\n", token);
        return CompilationError::SyntaxError;
    }

    #undef COM_DEF

    lstWrite(asmData, "%04x %-15.15s ", asmData->ip.value, line);
    LOG_MESSAGE_F(INFO, "Command: %s\n", token);

// ####################### ARGUMENTS ##########################################
    proc_arg_t args[MAX_ARGS] = {};
    int nArg = 0;

    if(command.flags.argImm){             //Has argument
        command.flags.argImm = 0;
        command.flags.argReg = 0;
        command.flags.argMem = 0;
        err = assemblyArguments(asmData, args, &nArg, &command, &curChr);
    }

    if(err != CompilationError::noErr){
        return err;
    }

//######################## OUTPUT ###############################################s
    lstWrite(asmData, "%02X ", (unsigned char)command.value);
    asmData->code[asmData->ip.value] = command.value;
    asmData->ip.commandPtr++;

    memcpy(asmData->code + asmData->ip.value, args, sizeof(proc_arg_t) * (size_t)nArg);
    asmData->ip.argPtr += nArg;

    lstWriteBytes(asmData, (char*)args, sizeof(proc_arg_t) * (size_t)nArg);

    if(!isEndStr(curChr)){
        LOG_DEBUG_F("Ends with:%s\n", curChr);
        return CompilationError::SyntaxError;
    }

    lstWrite(asmData, "\n");
    return err;
}
//---------------------------------------------------------------------------------

void lstWrite(AsmData* asmData, const char* format, ...){
    LOG_ASSERT(asmData != NULL);
    LOG_ASSERT(format != NULL);

    if(asmData->nPass != ASM_PASSES - 1)
        return;

    va_list args;
    va_start(args, format);
    vfprintf(asmData->lstFile, format, args);
    va_end(args);
}

//---------------------------------------------------------------------------------

void lstWriteBytes(AsmData* asmData, const char* data, const size_t n){
    LOG_ASSERT(asmData != NULL);
    LOG_ASSERT(data != NULL);

    for(size_t i = 0; i < n; ++i)
        lstWrite(asmData, "%02X ", (unsigned char)data[i]);
}

//---------------------------------------------------------------------------------

void asmDataCtor(AsmData* asmData, const char* filename){
    LOG_ASSERT(asmData  != NULL);
    LOG_ASSERT(filename != NULL);

    asmData->code = (char*)calloc(PROGRAM_MIN_SZ, sizeof(char));
    asmData->capacity = PROGRAM_MIN_SZ;

    LOG_ASSERT(asmData->code != NULL);

    asmData->labels = (Label*)calloc(MAX_LABELS, sizeof(Label));
    asmData->nLabels = 0;

    LOG_ASSERT(asmData->labels != NULL);

    asmData->lstFile = createOutFile(filename, LST_FORMAT);
    LOG_ASSERT(asmData->lstFile != NULL);

    asmData->ip.value = 0;
    asmData->nPass = 0;
}

//---------------------------------------------------------------------------------

void asmDataDtor(AsmData* asmData){
    free(asmData->code);
    free(asmData->labels);
    if(asmData->lstFile)
        fclose(asmData->lstFile);
}

//---------------------------------------------------------------------------------

int isToExpand(const AsmData* asmData){
    LOG_ASSERT(asmData != NULL);
    return (asmData->capacity - asmData->ip.value <= TO_EXPAND);
}

//---------------------------------------------------------------------------------

void expandData(AsmData* asmData){
    LOG_ASSERT(asmData != NULL);

    char* newPtr = (char*)realloc(asmData->code, (asmData->capacity *= EXPAND_COEF) * sizeof(char));
    LOG_ASSERT(newPtr != NULL);

    asmData->code = newPtr;
}

//---------------------------------------------------------------------------------

void registerLabel(AsmData* asmData, const char* name, proc_instruction_ptr_t ip){
    LOG_ASSERT(name != NULL);
    LOG_ASSERT(asmData != NULL);
    if(asmData->nPass != 0)
        return;

    if(asmData->nLabels == MAX_LABELS){
        LOG_MESSAGE_F(ERROR, "Too many labels.\n");
        return;
    }

    LOG_MESSAGE_F(DEBUG, "Registring label: %s\n", name);

    for(size_t i = 0; i < asmData->nLabels; ++i){
        if(strcmp(name, asmData->labels[i].name) == 0){
            if(asmData->labels[i].ip.value == UNDEF_PT.value){
                asmData->labels[i].ip.value = ip.value;
                LOG_DEBUG_F("%04x\n", asmData->labels[i].ip.value);
            }
            else{
                LOG_MESSAGE_F(WARNING, "Redefining label: %s\n", name);
            }
            return;
        }
    }

    asmData->labels[asmData->nLabels].ip.value = ip.value;
    LOG_DEBUG_F("%X\n", asmData->labels[asmData->nLabels].ip.value);
    
    if(strlen(name) > MAX_LABEL_LEN){
        LOG_MESSAGE_F(ERROR, "Too long label name");
        return;
    }
    
    strcpy(asmData->labels[asmData->nLabels].name, name);
    asmData->nLabels++;
}

//---------------------------------------------------------------------------------

proc_instruction_ptr_t getLabel(AsmData* asmData, const char* name){
    LOG_ASSERT(name != NULL);
    LOG_ASSERT(asmData != NULL);
    
    LOG_MESSAGE_F(DEBUG, "Finding label from %d: %s\n", asmData->nLabels ,name);

    for(size_t i = 0; i < asmData->nLabels; ++i){
        LOG_DEBUG_F("%d: %s %s %X\n",i, name, asmData->labels[i].name, asmData->labels[i].ip.value);
        if(strcmp(name, asmData->labels[i].name) == 0){
            return asmData->labels[i].ip;
        }
    }

    if(asmData->nPass == 0){
        registerLabel(asmData, name, UNDEF_PT);
    }
    else{
        LOG_MESSAGE_F(ERROR, "No label defenition\n");
    }

    return UNDEF_PT;
}

//------------------------------------------------------------------------------

int isLabel(char* s, proc_arg_t *value, AsmData* asmData){
    LOG_ASSERT(asmData != NULL);

    if(s == NULL || *s == '\0')
        return 0;
    
    size_t len = strlen(s);
    
    if(!isalpha(*s) && s[0] != '_')
        return 0;
    
    for(size_t i = 1; i < len - 1; ++i){
        if(!isalnum(s[i]) && s[i] != '_')
            return 0;
    }

    if(s[len - 1] != ':'){
        return 0;
    }

    s[len - 1] = '\0';

    if(value != NULL)
        *value = getLabel(asmData, s).asArg;

    return 1;
}

//------------------------------------------------------------------------------

int isRegister(const char* s, proc_arg_t* value){
    if(s == NULL || *s == '\0')
        return 0;
    
    size_t len = strlen(s);

    if(len != 2)
        return 0;
    
    if(s[1] != 'x')
        return 0;
    
    if(s[0] < 'a' || s[0] >='a' + NREGS)
        return 0;
    
    if(value != NULL)
        *value = s[0] - 'a' + 1;
    return 1;
}

//------------------------------------------------------------------------------

int isImmediate(const char* s, proc_arg_t* value){
    if(s == NULL || *s == '\0')
        return 0;

    size_t len   = strlen(s);
    size_t nRead = 0;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat="
    if(value != NULL)
        sscanf(s, "%d%n", value, &nRead);
    #pragma GCC diagnostic pop

    return nRead == len;
}

//------------------------------------------------------------------------------

CompilationError parseArgument(AsmData* asmData, proc_command_t* command, char* argStr, proc_arg_t* argVal, int nArgs){
    LOG_ASSERT(asmData != NULL);
    LOG_ASSERT(command != NULL);
    LOG_ASSERT(argStr  != NULL);
    LOG_ASSERT(argVal  != NULL);

    if(!command->flags.argImm && isImmediate(argStr, argVal)){
        command->flags.argImm = 1;
        return CompilationError::noErr;
    }

    if(!command->flags.argReg && isRegister(argStr, argVal)){
        command->flags.argReg = 1;
        return CompilationError::noErr;
    }

    if(!command->flags.argImm && isLabel(argStr, argVal, asmData)){
        command->flags.argImm = 1;
        *argVal -= asmData->ip.value + nArgs * sizeof(proc_arg_t) + sizeof(proc_command_t);
        return CompilationError::noErr;
    }
    return CompilationError::SyntaxError;
}

//--------------------------------------------------------------------------------------

CompilationError assemblyArguments(AsmData* asmData, proc_arg_t args[MAX_ARGS], int* nArg, proc_command_t* command, char** curChr){
    LOG_ASSERT(asmData != NULL);
    LOG_ASSERT(args    != NULL);
    LOG_ASSERT(nArg    != NULL);
    LOG_ASSERT(curChr  != NULL);
    LOG_ASSERT(*curChr != NULL);
    LOG_ASSERT(command != NULL);

    CompilationError err = CompilationError::noErr;
    char argStr[MAX_ARGS][MAX_TOKEN_LEN] = {};
    memset(argStr, 0, MAX_ARGS*MAX_TOKEN_LEN*sizeof(char)); 
    int nRead = 0;
    *nArg = sscanf(*curChr, " [ %"TOKEN"%n + %"TOKEN"]%n", argStr[0], &nRead, argStr[1], &nRead);

    if(*nArg > 0){
        command->flags.argMem = 1;
        if(*nArg == 1){
            if(*(*curChr + nRead) != ']'){
                err = CompilationError::SyntaxError;
            }
            else nRead++;
        }
    }
    else{
        *nArg = sscanf(*curChr, " %"TOKEN"%n + %"TOKEN"%n", argStr[0], &nRead, argStr[1], &nRead);
    }

    *curChr += nRead;
    LOG_DEBUG_F("%d \"%s\" \"%s\"\n", *nArg, argStr[0], argStr[1]);

    if(*nArg <= 0){
        return CompilationError::SyntaxError;
    }

    // LOG_DEBUG_F("ComVal: %02X \n", (unsigned char)command->value);
    err = parseArgument(asmData, command, argStr[0], args, *nArg);
    // LOG_DEBUG_F("ComVal: %02X \n", (unsigned char)command->value);

    if(*nArg == 2 && err == CompilationError::noErr){
        int toSwap = command->flags.argImm;
        err = parseArgument(asmData, command, argStr[1], args + 1, *nArg);
        // LOG_DEBUG_F("ComVal: %02X \n", (unsigned char)command->value);
        if(toSwap){
            swapInt(args, args + 1);
        }
    }
    LOG_DEBUG_F("Arg Err:%d\n", err);
    return err;
}