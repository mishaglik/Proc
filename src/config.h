#ifndef PROC_CONFIG_H
#define PROC_CONFIG_H

//############# Work mode #######################

#define VIDEO
// #define STEP_MODE
// #define DUMPING
#define LISTENIG_FILE

// #define HOLLOW_RECT

//############### Hardware ######################
#define NREGS  5 //0, a, b, c, d
#define RAM_SZ 1024

#define MAX_LABEL_LEN 32
#define STACK_MAX_SZ  10240
#define MAX_PROC_OPERATIONS 1000000

#define DISPLAY_WIDHT  100
#define DISPLAY_HEIGHT 100


//############### Other constants ###############
#define OUT_FORMAT  ".out"
#define LST_FORMAT  ".lst"


#define PROGRAM_MIN_SZ  1024
#define EXPAND_COEF     2
#define TO_EXPAND       (4 * sizeof(int))
#define MAX_LABELS      1024
#define MAX_ARGS        2

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
#define SIGNATURE 4466256 /*'D&P'*/
#pragma GCC diagnostic pop


#define PIXEL_SZ 5

#endif