#ifndef PROC_PROC_T_H
#define PROC_PROC_T_H
#include <stdlib.h>
#include "../lib/version.h"

struct FileHeader{
    int signature  = 0;
    int mj_version = 0;
    int mn_verison = 0;
    int bd_version = 0;
};
const int SIGNATURE = 'D&P';
const FileHeader BIN_FILE_HEADER = {SIGNATURE, MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION};
const char* OUT_FORMAT = ".out";

struct proc_command_t
{
    unsigned char id    :4;
    unsigned char isJump:1;
    unsigned char argImm:1;
    unsigned char argReg:1;
    unsigned char argMem:1;
};

typedef int proc_arg_t;
union proc_instruction_ptr_t
{
    int             value;
    proc_command_t* commandPtr;
    proc_arg_t*     argPtr;
};

typedef unsigned int proc_err_t;

proc_command_t numToCom(const char num){
    return *(proc_command_t*)&num;
}

#endif