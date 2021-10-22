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

int isEndStr(const char* s){
    return sscanf(s, "%*s") == EOF;
}

void swapInt(int* p1, int* p2){
    int tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}