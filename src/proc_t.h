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

union proc_command_t
{   
    unsigned char value;
    struct{
    unsigned char id    :4;
    unsigned char isJump:1;
    unsigned char argImm:1;
    unsigned char argReg:1;
    unsigned char argMem:1;
    } flags;
};

typedef int proc_arg_t;
union proc_instruction_ptr_t
{
    proc_arg_t      value;
    proc_command_t* commandPtr;
    proc_arg_t*     argPtr;
};

typedef unsigned int proc_err_t;


#endif