#include "Assembler.h"
#include "../commands.h"
#include "../lib/text.h"
#include "../lib/Logger.h"
#include <string.h>

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
    char* outFilename = (char*)calloc(sizeof(filename) + sizeof(outputFormat), sizeof(char));
    strcat(outFilename, filename);

    char* lastPt = strrchr(outFilename, '.');
    if(lastPt != NULL)
        *lastPt = '\0';

    strcat(outFilename, outputFormat);

    FILE* outFile = fopen(outFilename, "w");
}