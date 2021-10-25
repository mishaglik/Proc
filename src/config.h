#ifndef PROC_CONFIG_H
#define PROC_CONFIG_H

#define NREGS  5 //0, a, b, c, d
#define RAM_SZ 1024

#define MAX_LABEL_LEN 32

#define DISPLAY_WIDHT  100u
#define DISPLAY_HEIGHT 100u

#define VIDEO
#define STEP_MODE
#define DUMPING
#define LISTENIG_FILE

// #define HOLLOW_RECT


#define OUT_FORMAT  ".out"
#define LST_FORMAT  ".lst"


const size_t PROGRAM_MIN_SZ = 1024;
const size_t EXPAND_COEF    = 2;
const size_t TO_EXPAND      = 4 * sizeof(int);
const size_t MAX_LABELS     = 1024;
const size_t MAX_ARGS       = 2;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
// const int SIGNATURE = 'D&P';
const int SIGNATURE = 4466256;
#pragma GCC diagnostic pop

#define NREGS 5 //0, a, b, c, d

#endif