#ifndef PROC_CONFIG_H
#define PROC_CONFIG_H
#include <stdlib.h>
//############# Work mode #######################

// #define VIDEO
// #define STEP_MODE
// #define DUMPING

#define LISTENIG_FILE

#define HOLLOW_RECT

//############### Hardware ######################
const size_t NREGS                  = 5; //0, a, b, c, d
const size_t RAM_SZ                 = 8192;

const size_t MAX_TOKEN_LEN          = 32;
const size_t MAX_LABEL_LEN          = MAX_TOKEN_LEN;
const size_t STACK_MAX_SZ           = 10240;
const size_t MAX_PROC_OPERATIONS    = 1000000;

const size_t DISPLAY_WIDHT          = 100;
const size_t DISPLAY_HEIGHT         = 100;


//############### Other constants ###############
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static const char* OUT_FORMAT = ".out";
static const char* LST_FORMAT = ".lst";
#pragma GCC diagnostic pop

const size_t PROGRAM_MIN_SZ  = 1024;
const size_t EXPAND_COEF     = 2;
const size_t TO_EXPAND       = (4 * sizeof(int));
const size_t MAX_LABELS      = 1024;
const size_t MAX_ARGS        = 2;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"

const unsigned SIGNATURE = 0x00442650; /*'D&P'*/
#pragma GCC diagnostic pop
//TODO: Def

const size_t PIXEL_SZ = 5;

const size_t LAYOUT_BEGIN_PROG = 256;

#endif