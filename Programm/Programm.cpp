#include "Programm.h"
#include "../lib/Logger.h"
#include "../lib/File.h"



Programm* freadProgramm(const char* filename){
    LOG_ASSERT(filename != NULL);

    FILE* file = fopen(filename, "rb");
    if(file == NULL){
        LOG_MESSAGE_F(ERROR,"File %s doen't exists.", filename);
        return NULL;
    }

    size_t fsize = getFileSize(file);

    FileHeader header = {};
    if(fsize < sizeof(header) || fread(&header, sizeof(FileHeader), 1, file) != sizeof(FileHeader)){
        LOG_MESSAGE_F(ERROR, "File \"%s\" is too small", filename);
        return NULL;
    }
    fsize -= sizeof(FileHeader);

    if(header.signature != SIGNATURE || header.mj_version != MAJOR_VERSION){
        LOG_MESSAGE_F(ERROR, "File \"%s\" has incopatiable header", filename);
        return NULL;
    }

    Programm* programm = createProgramm(fsize);

    fread(programm->data, sizeof(char), fsize, file);
    
    fclose(file);
    
    return programm;
}


//-----------------------------------------------------------------------------------------------------------------------------

void fwriteProgramm(const char* filename, Programm* programm){
    LOG_ASSERT(filename       != NULL);
    LOG_ASSERT(programm       != NULL);
    LOG_ASSERT(programm->data != NULL);

    FILE* file = fopen(filename, "wb");
    LOG_ASSERT(filename != NULL);

    LOG_ASSERT(fwrite(&FILE_HEAD, sizeof(FileHeader), 1, file));

    LOG_ASSERT(programm->proc_cnt < programm->size);
    LOG_ASSERT(fwrite(programm->data, sizeof(char), programm->proc_cnt, file) == programm->proc_cnt);
    
    fclose(file);
}

//------------------------------------------------------------------------------------------------------------------------------

Programm* createProgramm(size_t size){
    Programm* programm = (Programm*)calloc(1, sizeof(Programm));
    LOG_ASSERT(programm != NULL);
    
    programm->data = (char*)calloc(size, sizeof(char));
    programm->size = size;

    return programm;
}

//------------------------------------------------------------------------------------------------------------------------------

void freeProgramm(Programm** programm){
    LOG_ASSERT(programm != NULL);

    if((*programm) != NULL)
        free((*programm)->data);
    
    free(*programm);

    *programm = NULL;
}

//------------------------------------------------------------------------------------------------------------------------------

void expandProgrammData(Programm* programm){
    LOG_ASSERT(programm != NULL);

    char* newData = (char*)realloc(programm->data, programm->size * PROG_EXPAND_COEF * sizeof(char));
    LOG_ASSERT(newData != NULL);    //TODO: better realooc problem

    programm->data  = newData;
    programm->size *= PROG_EXPAND_COEF;
}