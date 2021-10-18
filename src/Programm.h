#ifndef PROC_PROGRAMM_H
#define PROC_PROGRAMM_H
#include "stdlib.h"
#include "utils.h"
#include "../lib/version.h"
const int SIGNATURE = 'D&P';

const size_t INIT_PROGRAMM_SIZE = 1024;
const size_t PROG_EXPAND_COEF   = 2;

struct FileHeader{
    int signature  = 0;
    int mj_version = 0;
    int mn_verison = 0;
    int bd_version = 0;
};

const FileHeader FILE_HEAD = {SIGNATURE, MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION};

struct Programm
{    
    char* data = NULL;

    size_t proc_cnt = 0;    //index of command. Works as programm size in assembly mode.
    size_t size = 0;        //Size of data 
};

/**
 * @brief Create programm reading it from file.
 * 
 * @param filename - name of file to read.
 * @return Programm* - ptr to programm.
 */
Programm* freadProgramm(const char* filename);

/**
 * @brief Writes programm to file.
 * 
 * @param filename - name of output file.
 * @param programm - programm to dump.
 */
void fwriteProgramm(const char* filename, Programm* programm);


/**
 * @brief Create a Programm object
 * 
 * @return Programm* 
 */
Programm* createProgramm(size_t size = INIT_PROGRAMM_SIZE);

/**
 * @brief free's programm. 
 * Do not use it after that action.
 * @param programm 
 */
void freeProgramm(Programm** programm);

/**
 * @brief Expands data of programm at coef 
 * 
 * @param Programm 
 */
void expandProgrammData(Programm* programm);
#endif
