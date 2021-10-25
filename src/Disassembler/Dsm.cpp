#include "Dsm.h"
#include <stdarg.h>
#include <string.h>

const size_t DSM_WALKS = 2;

void disassemblyFile(const char* filename){
    LOG_ASSERT(filename != NULL);
    //############## Reading file ###########################################
    LOG_MESSAGE_F(INFO, "Disassemblying file: %s\n", filename);

    char* code = NULL;

    FILE* inFile = fopen(filename, "rb");
    LOG_ASSERT(inFile != NULL);

    size_t fileSz = getFileSize(inFile);

    FileHeader hdr = {};
    fread(&hdr, sizeof(FileHeader), 1, inFile);
    fileSz -= sizeof(FileHeader); 

    if(hdr.signature != SIGNATURE || hdr.mj_version != MAJOR_VERSION){
        LOG_MESSAGE_F(FATAL, "Inappropiate file header");
        return;
    }

    code = (char*) calloc(fileSz + 1, sizeof(char));
    LOG_ASSERT(code != NULL);

    fread(code, sizeof(char), fileSz, inFile);
    fclose(inFile);

    FILE* outFile = createOutFile(filename, ".dsm");
    LOG_ASSERT(outFile != NULL);

    proc_instruction_ptr_t labels[MAX_LABELS] = {};
    size_t curLabel = 0;

//###################### Disasemblying #####################
    for(size_t nWalk = 0; nWalk < DSM_WALKS; ++nWalk){
        proc_instruction_ptr_t ip = {0};
        proc_command_t command = {0};

        while(ip.value < fileSz){
            if(isLabeled(labels, ip, curLabel)){
                dsmPrint(nWalk, outFile, "LABEL_%04x:\n", ip.value);
            }

            command.value = code[ip.value];
            ip.commandPtr++;
            const char* commandName = NULL;

            LOG_DEBUG_F("Command %02X\n", command.value);
            
            switch (command.value & NUM_MASK)
            {
            #define COM_DEF(name, num, code, ...) \
                case (num & NUM_MASK):            \
                    commandName = #name;          \
                    break;

            #include "../commands.h"

            #undef COM_DEF
            default:
                LOG_MESSAGE_F(ERROR, "Unknown command");
                return;
                break;
            }
            dsmPrint(nWalk, outFile, "%s ", commandName);
            if(command.flags.argMem)
                dsmPrint(nWalk, outFile, "[");

            if(command.flags.argReg){
                dsmPrint(nWalk, outFile, "%cx",'a' + code[ip.value] - 1);
                ip.argPtr++;
            }

            if(command.flags.argImm){
                if(command.flags.argReg)
                    dsmPrint(nWalk, outFile, " + ");

                if(isLabelCommand(commandName)){
                    dsmPrint(nWalk, outFile, "LABEL_%04x:", *(proc_arg_t*)(code + ip.value));
                    putLabel(labels, &curLabel, {*(proc_arg_t*)(code + ip.value)}, nWalk);
                }
                else{
                    dsmPrint(nWalk, outFile, "%d", *(proc_arg_t*)(code + ip.value));
                }

                ip.argPtr++;
            }

            if(command.flags.argMem)
                dsmPrint(nWalk, outFile, "]");
            dsmPrint(nWalk, outFile, "\n");
        }
    }
}

//------------------------------------------------------------------------

int isLabelCommand(const char* name){
    LOG_ASSERT(name != NULL);
    return (name[0] == 'j') || (strcmp(name, "call") == 0);
}

//------------------------------------------------------------------------

void dsmPrint(size_t nWalk, FILE* file, const char* format, ...){
    LOG_ASSERT(format != NULL);
    LOG_ASSERT(file != NULL);

    if(nWalk != DSM_WALKS - 1)
        return;
    
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
}

void putLabel(proc_instruction_ptr_t labels[MAX_LABELS], size_t* curLabel, proc_instruction_ptr_t ip, size_t nWalk){
    LOG_ASSERT(labels != NULL);
    LOG_ASSERT(curLabel != 0);

    if(nWalk != 0){
        return;
    }

    if(*curLabel == MAX_LABELS)
        return;
    
    for(size_t i = 0; i < *curLabel; ++i){
        if(labels[i].value == ip.value)
            return;
    }
    labels[(*curLabel)++] = ip;
    return;
}

int isLabeled(proc_instruction_ptr_t labels[MAX_LABELS], proc_instruction_ptr_t curIp, size_t curLabel){
    LOG_ASSERT(labels != NULL);

    for(size_t i = 0; i < curLabel; ++i){
        if(labels[i].value == curIp.value){
            return 1;
        }
    }
    return 0;
}