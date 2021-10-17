#include "Assembler.h"
#include <string.h>

extern const FileHeader asmHeadPtr;

extern const char* outputFormat;
extern const int SIGNATURE;

const size_t PROGRAM_MIN_SZ = 1024;

void assemblyFile(const char* filename){
    LOG_ASSERT(filename != NULL);

    LOG_MESSAGE_F(INFO, "Beginning assemblying file %s\n", filename);
    
    FILE* inFile = fopen(filename, "r");
    if(inFile == NULL){
        LOG_MESSAGE_F(ERROR, "Error opening file: %s\n", filename);
        return;
    }
    Text programm_txt = {};
    
    LOG_DEBUG("Reading text");
    readTextf(inFile, &programm_txt);
    fclose(inFile);

    LOG_DEBUG("Parsing text");
    parseText(&programm_txt);

    Programm* programm = createProgramm();

    for(size_t line = 0; line < programm_txt.nStrings; ++line){
        // LOG_MESSAGE_F(DEBUG, "Assemblying line %d ...", line);

        assemblyLine(programm, programm_txt.strings + line);
        
        // LOG_MESSAGE_F(NO_CAP, "Successfull\n");
    }

    freeText(&programm_txt);

    char* outFilename = makeOutFilename(filename);
    LOG_ASSERT(outFilename != NULL);

    fwriteProgramm(outFilename, programm);
    free(outFilename);

    freeProgramm(&programm);
    LOG_MESSAGE_F(INFO, "End assemblying file %s\n\n", filename);
}

//------------------------------------------------------------------------------------------------------------------------

char* makeOutFilename(const char* filename){
    LOG_ASSERT(filename != NULL);

    char* outFilename = (char*)calloc(sizeof(filename) + sizeof(outputFormat), sizeof(char));
    strcat(outFilename, filename);

    char* lastPt = strrchr(outFilename, '.');
    if(lastPt != NULL)
        *lastPt = '\0';

    strcat(outFilename, outputFormat);

    return outFilename;
}                                                                      

//------------------------------------------------------------------------------------------------------------------------

void assemblyLine(Programm* programm, const String* line){
    LOG_ASSERT(line != NULL);
    LOG_ASSERT(programm != NULL);

    //TODO: Do it better. Very crutchful;
    char* commSym = strchr(line->pBegin, ';');
    if(commSym != NULL){
        *commSym = '\0';
    }

    char* commandStr = NULL;
    proc_arg_t argument = 0;
    sscanf(line->pBegin,"%ms %d", &commandStr, &argument);
    
    if(commandStr == NULL)
        return;
    ProcCommand cur_command = (ProcCommand)(-1);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define STR_TO_COM(command)                                     \
    if(strcmp(#command, commandStr) == 0) {                     \
        cur_command = ProcCommand::command;                     \
    }                                                           
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    COMMAND_APPLY(STR_TO_COM);
#undef STR_TO_COM
    free(commandStr);


    //TODO: Add listening
    LOG_ASSERT(programm->proc_cnt <= programm->size);

    if(programm->proc_cnt == programm->size)
        expandProgrammData(programm);


    programm->data[programm->proc_cnt++] = (char)cur_command;

    if(hasArgument(cur_command)){
        if(programm->proc_cnt + sizeof(proc_arg_t) == programm->size)
            expandProgrammData(programm);

        *(proc_arg_t*)(programm->data + programm->proc_cnt) = argument;
        programm->proc_cnt += sizeof(proc_arg_t);
    }
}

//---------------------------------------------------------------------------------


