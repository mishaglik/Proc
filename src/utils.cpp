#include <string.h>
#include "utils.h"
#include "../lib/Logger.h"

FILE* createOutFile(const char* filename, const char* outputFileExt){
    LOG_ASSERT(filename != NULL);

    char* outFilename = (char*)calloc(sizeof(filename) + sizeof(outputFileExt), sizeof(char));
    strcat(outFilename, filename);

    char* lastPt = strrchr(outFilename, '.');
    if(lastPt != NULL)
        *lastPt = '\0';

    strcat(outFilename, outputFileExt);

    FILE* file = fopen(outFilename, "w");
    LOG_ASSERT(file != NULL);

    free(outFilename);

    return file;
}

proc_command_t numToCom(char num){
    return *(proc_command_t*)&num;
}