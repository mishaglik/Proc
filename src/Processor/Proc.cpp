#include "Proc.h"
#include <unistd.h>
#include "Proc_DSL.h"
#include <math.h>

int processorLoad(Processor* proc, const char* filename){
    LOG_ASSERT(proc != NULL);
    LOG_ASSERT(filename != NULL);

    proc->ip.value = 0;

    FILE* inFile = fopen(filename, "rb");
    LOG_ASSERT(inFile != NULL);

    size_t fileSz = getFileSize(inFile);

    FileHeader hdr = {};
    fileSz -= fread(&hdr, sizeof(FileHeader), 1, inFile);

    if(hdr.signature != SIGNATURE || hdr.mj_version != MAJOR_VERSION){
        LOG_MESSAGE_F(FATAL, "Inappropiate file header");
        return -1;
    }

    proc->code = (char*) calloc(fileSz + 1, sizeof(char));
    LOG_ASSERT(proc->code != NULL);
    proc->code_sz = fileSz + 1;

    fread(proc->code, sizeof(char), fileSz, inFile);
    fclose(inFile);

    LOG_ASSERT(!stack_init(&(proc->stack)));
#ifdef VIDEO
    LOG_ASSERT(!initVideo(&(proc->videoDriver)));
#endif
    proc->status = ProcStatus::Idle;
    
    return 0;
}


RuntimeError processorExecute(Processor* proc){
    LOG_ASSERT(proc != NULL);
    LOG_ASSERT(proc->status == ProcStatus::Idle);

    proc->status = ProcStatus::Running;
    
    proc_command_t command = {'\xFF'};

    size_t cntWhile = 0;

    while (proc->status == ProcStatus::Running)
    {
        #ifdef DUMPING
        procDump(proc);
        #endif
        command.value = proc->code[proc->ip.value];
        proc->ip.commandPtr++;
        proc_arg_t* argument = NULL;            
        proc_arg_t tmp1 = 0, tmp2 = 0;          
        proc_arg_t immArg = 0;      
        #ifdef DUMPING
        LOG_DEBUG_F("Command %02X: %s\n", command.value, commandName(command));
        #endif
        #ifdef STEP_MODE
            PAUSE;
        #endif
        switch (command.value & NUM_MASK)
        {
        #define COM_DEF(name, num, code, ...)                                           \
            case (num & NUM_MASK):                                                      \
                if(getArg(proc, &argument, command, &immArg) != RuntimeError::noErr){   \
                    return RuntimeError::MemoryAccessErr;                               \
                }                                                                       \
                {code}                                                                  \
                break;                                                                  \

        #include "../commands.h"

        #undef COM_DEF
        default:
            proc->status = ProcStatus::Error;
            LOG_MESSAGE_F(ERROR, "Unknown command, aborting");
            return RuntimeError::UnknownCommand;
            break;
        }
    cntWhile++;
    }
    return RuntimeError::noErr;
}

//-----------------------------------------------------------------------------------------------

RuntimeError getArg(Processor* proc, proc_arg_t** arg, proc_command_t command, proc_arg_t* immArg){
    LOG_ASSERT(proc     != NULL);
    LOG_ASSERT(arg      != NULL);
    LOG_ASSERT(immArg   != NULL);

    *arg = NULL;

    if(command.flags.argReg){
        *arg = proc->reg + proc->code[proc->ip.value];
        proc->ip.argPtr++;
    }    
    if(command.flags.argImm){
        *immArg = (*arg ? **arg : 0) + *(proc_arg_t*)(proc->code + proc->ip.value);
        proc->ip.argPtr++;
        *arg = immArg;
    }
    if(command.flags.argMem){
        if(!*arg){
            return RuntimeError::MemoryAccessErr;
        }
        RuntimeError err = RAM_getPtr(proc, **arg, arg);

        if(err != RuntimeError::noErr)
            return err;
    }
    return RuntimeError::noErr;
}

RuntimeError RAM_getPtr(Processor* proc, proc_arg_t address, proc_arg_t** arg){
    LOG_ASSERT(proc != NULL);
    LOG_ASSERT(arg != NULL);

    if(address <= 0)
        return RuntimeError::MemoryAccessErr;

    if(address < RAM_SZ){
        *arg = proc->ram.data + address;
        return RuntimeError::noErr;
    }
    address-=RAM_SZ;
#ifdef VIDEO
    if(address < DISPLAY_WIDHT * DISPLAY_HEIGHT){
        *arg = getVideoMemPtr(&(proc->videoDriver), address);
        return RuntimeError::noErr;
    }
#endif
    return RuntimeError::MemoryAccessErr;
}

void processorFree(Processor* proc){
    stack_free(&proc->stack);
    proc->status = ProcStatus::OFF;
#ifdef VIDEO
    finishVideo(&(proc->videoDriver));
#endif
    free(proc->code);
}

void procDump(Processor* proc){
    LOG_ASSERT(proc != NULL);

    LOG_DEBUG_F("######### Processor dump ######\n")
    LOG_DEBUG_F("Processor status: %d \n", proc->status);
    LOG_DEBUG_F("IP value: %04x\n", proc->ip);
    LOG_DEBUG_F("Proc code dump:\n");
    dumpBytes(proc->code + MAX(0 , proc->ip.asArg - 25), MIN(50, proc->code_sz - MAX(0, proc->ip.asArg - 25)), proc->ip.value - MAX(0, proc->ip.asArg - 25));
    LOG_DEBUG_F("Proc reg dump\n");
    LOG_DEBUG_F("#############################################\n");
    LOG_DEBUG_F("# %8d # %8d # %8d # %8d #\n", proc->reg[1], proc->reg[2], proc->reg[3], proc->reg[4]);
    LOG_DEBUG_F("#############################################\n");
    LOG_DEBUG_F("Proc mem dump\n");
    dumpMem(proc->ram.data, 20);
    LOG_DEBUG_F("Proc stack dump:\n");
    dumpStack(&proc->stack);
}

void dumpBytes(char* data, size_t n, size_t toColor){
    LOG_ASSERT(data != NULL);
    LOG_DEBUG_F("#");
    for(size_t i = 0; i < n; ++i){
        LOG_MESSAGE_F(NO_CAP, "###");
    }
    LOG_MESSAGE_F(NO_CAP, "#\n");
    LOG_MESSAGE_F(DEBUG, "#");
    for(size_t i = 0; i < n; ++i){
        if(i == toColor){
            LOG_MESSAGE_F(NO_CAP, "\033[1;31m");
        }
        LOG_MESSAGE_F(NO_CAP, "%02X ", (unsigned char)data[i]);
        if(i == toColor){
            LOG_MESSAGE_F(NO_CAP, "\033[0m");
        }
    }
    LOG_MESSAGE_F(NO_CAP, "#\n");
    LOG_DEBUG_F("#");
    for(size_t i = 0; i < n; ++i){
        LOG_MESSAGE_F(NO_CAP, "###");
    }
    LOG_MESSAGE_F(NO_CAP, "#\n");
}

void dumpStack(Stack* stack){
    LOG_ASSERT(stack != NULL);

    LOG_DEBUG_F("############\n");
    for(size_t i = 0; i < MIN(stack->size, 10); ++i){
        LOG_DEBUG_F("# %8d #\n", stack->data[stack->size - 1 - i]);
    }
    LOG_DEBUG_F("############\n");

}

void dumpMem(proc_arg_t* data, size_t n){
    LOG_ASSERT(data != NULL);
    LOG_DEBUG_F("#");
    for(size_t i = 0; i < n; ++i){
        LOG_MESSAGE_F(NO_CAP, "###\033[32m%03x\033[0m###", i);
    }
    LOG_MESSAGE_F(NO_CAP, "#\n");
    LOG_MESSAGE_F(DEBUG, "#");
    for(size_t i = 0; i < n; ++i){
        // if(i == toColor){
            // LOG_MESSAGE_F(NO_CAP, "\033[1;31m");
        // }
        LOG_MESSAGE_F(NO_CAP, "%08X ", data[i]);
        // if(i == toColor){
            // LOG_MESSAGE_F(NO_CAP, "\033[0m");
        // }
    }
    LOG_MESSAGE_F(NO_CAP, "#\n");
    LOG_DEBUG_F("#");
    for(size_t i = 0; i < n; ++i){
        LOG_MESSAGE_F(NO_CAP, "#########");
    }
    LOG_MESSAGE_F(NO_CAP, "#\n");
}