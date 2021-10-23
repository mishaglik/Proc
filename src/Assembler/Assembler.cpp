#include "Assembler.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define LEXEM "[A-Za-z0-9:_-]"

const char* OUT_FORMAT = ".out";
const char* LST_FORMAT = ".lst";

extern const int SIGNATURE;

const size_t PROGRAM_MIN_SZ = 1024;
const size_t EXPAND_COEF    = 2;
const size_t TO_EXPAND      = 4 * sizeof(int);
const size_t MAX_LABELS     = 1024;
const size_t MAX_ARGS       = 2;

const proc_instruction_ptr_t UNDEF_PT = {-1};

const int ASM_WALKS = 2;

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

    char* commentSym = strchr(line, ';');
    if(commentSym != NULL){
        *commentSym = '\0';
    }

    char* lexem  = NULL;
    char* curChr = line;
    int   nRead  = 0;

    sscanf(curChr, " %m"LEXEM"%n", &lexem, &nRead);
    curChr += nRead;
    if(lexem == NULL || *lexem == '\0'){
        LOG_MESSAGE_F(INFO, "Skipped: %s\n", line);
        return isEndStr(line) ? CompilationError::noErr : CompilationError::SyntaxError;
    }
//########################## Directives #######################################
    
    if(isLabel(lexem, NULL, asmData)){
        LOG_MESSAGE_F(INFO, "Label: %s\n", lexem);
        registerLabel(asmData, lexem, asmData->ip);
        lstWrite(asmData, "%04x %s\n", asmData->ip.value, lexem);
        free(lexem);
        return isEndStr(curChr) ? CompilationError::noErr : CompilationError::SyntaxError;
    }

//########################## Commands   #######################################
    proc_command_t command = {};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define COM_DEF(name, val, ...)                                 \
    if(strcmp(#name, lexem) == 0) {                             \
        command.value = (char)val;                              \
    }else                                                       \
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    #include "../commands.h"
    /*else*/{
        LOG_MESSAGE_F(ERROR, "Error: Unknown command: %s\n", lexem);
        return CompilationError::SyntaxError;
    }
    #undef COM_DEF

    lstWrite(asmData, "%04x %-15.15s ", asmData->ip.value, line);
    LOG_MESSAGE_F(INFO, "Command: %s\n", lexem);

    free(lexem);
// ####################### ARGUMENTS ##########################################
    proc_arg_t args[MAX_ARGS] = {};
    
    int nArg = 0;

    if(command.flags.argImm){             //Has argument
        command.flags.argImm = 0;
        command.flags.argReg = 0;
        command.flags.argMem = 0;

        char* argStr1 = NULL;
        char* argStr2 = NULL;

        nArg = sscanf(curChr, " [ %m"LEXEM"%n + %m"LEXEM"]%n", &argStr1, &nRead, &argStr2, &nRead);

        if(nArg == 0){
            nArg = sscanf(curChr, " %m"LEXEM"%n + %m"LEXEM"%n", &argStr1, &nRead, &argStr2, &nRead);
        }
        else{
            command.flags.argMem = 1;
            if(nArg == 1){
                if(*(curChr + nRead) != ']'){
                    return CompilationError::SyntaxError;
                }
                else nRead++;
            }
        }
        curChr += nRead;
        LOG_DEBUG_F("%d %s %s\n", nArg, argStr1, argStr2);
        if(nArg <= 0){
            return CompilationError::SyntaxError;
        }

        if(parseArgument(asmData, &command, argStr1, args) != CompilationError::noErr)
            return CompilationError::SyntaxError;

        if(nArg == 2){
            int toSwap = command.flags.argImm;
            if(parseArgument(asmData, &command, argStr2, args + 1) != CompilationError::noErr)
                return CompilationError::SyntaxError;
            if(toSwap){
                swapInt(args, args + 1);
            }
        }
        free(argStr1);
        free(argStr2);
    }

    lstWrite(asmData, "%02X ", (unsigned char)command.value);
    asmData->code[asmData->ip.value] = command.value;
    asmData->ip.commandPtr++;

    memcpy(asmData->code + asmData->ip.value, args, sizeof(proc_arg_t) * nArg);
    asmData->ip.argPtr += nArg;

    lstWriteBytes(asmData, (char*)args, sizeof(proc_arg_t) * nArg);
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

    if(asmData->nWalk != ASM_WALKS - 1)
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

    asmData->lstFile = createOutFile(filename, LST_FORMAT);

    asmData->ip.value = 0;
    asmData->nWalk = 0;
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
    if(asmData->nWalk != 0)
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
                LOG_DEBUG_F("%X\n", asmData->labels[i].ip.value);
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
    if(asmData->nWalk == 0){
        registerLabel(asmData, name, UNDEF_PT);
    }
    else{
        LOG_MESSAGE_F(ERROR, "No label defenitio\n");
    }
    return UNDEF_PT;
}

//------------------------------------------------------------------------------

int isLabel(char* s, proc_arg_t *value, AsmData* asmData){
    if(s == NULL)
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
    LOG_ASSERT(value != NULL);
    if(s == NULL)
        return 0;
    
    size_t len = strlen(s);

    if(len != 2)
        return 0;
    
    if(s[1] != 'x')
        return 0;
    
    if(s[0] < 'a' || s[0] >='a' + NREGS)
        return 0;
    
    *value = s[0] - 'a' + 1;
    return 1;
}

//------------------------------------------------------------------------------

int isNumber(const char* s, proc_arg_t* value){
    LOG_ASSERT(value != NULL);
    if(s == NULL)
        return 0;

    size_t len   = strlen(s);
    size_t nRead = 0;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat="
    sscanf(s, "%d%n", value, &nRead);
    #pragma GCC diagnostic pop

    return nRead == len;
}

//------------------------------------------------------------------------------

CompilationError parseArgument(AsmData* asmData, proc_command_t* command, char* argStr, proc_arg_t* argVal){
    LOG_ASSERT(asmData != NULL);
    LOG_ASSERT(command != NULL);
    LOG_ASSERT(argStr  != NULL);
    LOG_ASSERT(argVal  != NULL);

    if(!command->flags.argImm && isNumber(argStr, argVal)){
        command->flags.argImm = 1;
        return CompilationError::noErr;
    }

    if(!command->flags.argReg && isRegister(argStr, argVal)){
        command->flags.argReg = 1;
        return CompilationError::noErr;
    }

    if(!command->flags.argImm && isLabel(argStr, argVal, asmData)){
        command->flags.argImm = 1;
        return CompilationError::noErr;
    }
    return CompilationError::SyntaxError;
}