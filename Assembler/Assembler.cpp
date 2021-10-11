#include "Assembler.h"
#include "../commands.h"
#include "../lib/text.h"
#include "../lib/Logger.h"
#include <string.h>

extern const FileHeader asmHeadPtr;

extern const char* outputFormat;
extern const int SIGNATURE;

void assemblyFile(const char* filename){
    LOG_ASSERT(filename != NULL);

    LOG_MESSAGE_F(INFO, "Beginning asseblying file %s", filename);
    
    FILE* inFile = fopen(filename, "r");
    if(inFile == NULL){
        LOG_MESSAGE_F(ERROR, "Error opening file: %s", filename);
        return;
    }
    FILE* outFile = makeOutFile(filename);
    LOG_ASSERT(outFile != NULL);

    Text programm = {};
    readTextf(inFile, &programm);

    parseText(&programm);
}

FILE* makeOutFile(const char* filename){
    LOG_ASSERT(filename != NULL);

    char* outFilename = (char*)calloc(sizeof(filename) + sizeof(outputFormat), sizeof(char));
    strcat(outFilename, filename);

    char* lastPt = strrchr(outFilename, '.');
    if(lastPt != NULL)
        *lastPt = '\0';

    strcat(outFilename, outputFormat);

    FILE* outFile = fopen(outFilename, "wb");
    free(outFilename);

    if(outFile == NULL){
        LOG_MESSAGE(ERROR,"Error creating out file");
        LOG_RAISE(ERROR);
        return NULL;
    }
    
    fwrite(&FILE_HEAD, sizeof(FileHeader), 1, outFile);

    return outFile;
}                                                                      

void assemblyLine(const String* line, FILE* outFile){
    LOG_ASSERT(line != NULL);
    LOG_ASSERT(outFile != NULL);

    //TODO: Do it better. Very crutchful;
    char* commSym = strchr(line->pBegin, ';');
    if(commSym != NULL){
        *commSym = '\0';
    }

    char* commandStr = NULL;
    proc_t argument = 0;
    sscanf(line->pBegin,"%ms %d", &commandStr, &argument);
    
    if(commandStr == NULL)
        return;

#define STR_TO_COM(command)                                     \
    if(strcmp(#command, commandStr) == 0) {                     \
        fprintf(outFile, "%d", (int)ProcCommand::command);      \
        if(hasArgument(ProcCommand::command))                   \
            fwrite(&argument, sizeof(proc_t), 1, outFile);      \
    } 
    COMMAND_APPLY(STR_TO_COM);

#undef STR_TO_COM
    free(commandStr);

}