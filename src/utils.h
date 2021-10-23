#include <stdio.h>
#include "proc_t.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

FILE* createOutFile(const char* filename, const char* outputFileExt);

proc_command_t numToCom(const char num);

int isEndStr(const char* s);

void swapInt(int* p1, int* p2);

const char* commandName(proc_command_t command);
