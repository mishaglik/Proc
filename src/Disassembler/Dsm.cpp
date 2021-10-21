#include "Dsm.h"

void disassemblyFile(const char* filename){
    LOG_ASSERT(filename != NULL);
    char* code = NULL;

    FILE* inFile = fopen(filename, "rb");
    LOG_ASSERT(inFile != NULL);

    size_t fileSz = getFileSize(inFile);

    FileHeader hdr = {};
    fileSz -= fread(&hdr, sizeof(FileHeader), 1, inFile);

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

    proc_instruction_ptr_t ip = {0};
    proc_command_t command = {0};

    while(ip.value < fileSz){
        command.value = code[ip.value];
        ip.commandPtr++;
        proc_arg_t* argument = NULL;
        LOG_DEBUG_F("Command %02X\n", command.value);
        switch (command.value & NUM_MASK)
        {
        #define COM_DEF(name, num, code, ...) \
            case (num & NUM_MASK):            \
                fprintf(outFile, #name);      \
                break;

        #include "../commands.h"

        #undef COM_DEF
        default:
            LOG_MESSAGE_F(ERROR, "Unknown command");
            return;
            break;
        }
        fprintf(outFile, " ");
        if(command.flags.isJump){
            fprintf(outFile, "label_%d\n", code[ip.value]);
            ip.argPtr++;
            continue;
        }
        if(command.flags.argMem)
            fprintf(outFile, "[");
        if(command.flags.argReg){
            fprintf(outFile, "%cx",'a' + code[ip.value] - 1);
            ip.argPtr++;
        }
        if(command.flags.argImm){
            if(command.flags.argReg)
                fprintf(outFile, " + ");
            fprintf(outFile, "%d", code[ip.value]);
            ip.argPtr++;
        }
        if(command.flags.argMem)
            fprintf(outFile, "]");
        fprintf(outFile, "\n");
    }
}