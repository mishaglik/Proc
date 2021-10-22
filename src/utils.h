#include <stdio.h>
#include "proc_t.h"

FILE* createOutFile(const char* filename, const char* outputFileExt);

proc_command_t numToCom(const char num);

int isEndStr(const char* s);

void swapInt(int* p1, int* p2);
