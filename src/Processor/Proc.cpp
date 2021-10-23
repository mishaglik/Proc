#include "Proc.h"


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

    fread(proc->code, sizeof(char), fileSz, inFile);
    fclose(inFile);

    LOG_ASSERT(!stack_init(&(proc->stack)));
    
    proc->status = ProcStatus::Idle;
    
    return 0;
}


RuntimeError processorExecute(Processor* proc){
    LOG_ASSERT(proc != NULL);
    LOG_ASSERT(proc->status == ProcStatus::Idle);

    proc->status = ProcStatus::Running;
    
    proc_command_t command = {'\xFF'};

 

    while (proc->status == ProcStatus::Running)
    {
        command.value = proc->code[proc->ip.value];
        proc->ip.commandPtr++;
        proc_arg_t* argument = NULL;            
        proc_arg_t tmp1 = 0, tmp2 = 0;          
        proc_arg_t immArg = 0;                  
        LOG_DEBUG_F("Command %02X\n", command.value);
        switch (command.value & NUM_MASK)
        {
        #include "Proc_DSL.h"
        #define COM_DEF(name, num, code, ...)                \
            case (num & NUM_MASK):                           \
                getArg(proc, &argument, command, &immArg);   \
                {code}                                       \
                break;                                       \

        #include "../commands.h"

        #undef COM_DEF
        default:
            proc->status = ProcStatus::Error;
            LOG_MESSAGE_F(ERROR, "Unknown command, aborting");
            return RuntimeError::UnknownCommand;
            break;
        }
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
        *immArg = (*arg ? **arg : 0) + proc->code[proc->ip.value];
        proc->ip.argPtr++;
        *arg = immArg;
    }
    if(command.flags.argMem){
        if(!*arg){
            return RuntimeError::MemoryAccessErr;
        }
        RuntimeError err = RAM_getPtr(&proc->ram, **arg, arg);
        if(err != RuntimeError::noErr)
            return err;
    }
    return RuntimeError::noErr;
}

RuntimeError RAM_getPtr(RAM* ram, proc_arg_t address, proc_arg_t** arg){
    LOG_ASSERT(ram != NULL);
    LOG_ASSERT(arg != NULL);

    if(address > RAM_SZ || address <= 0)
        return RuntimeError::MemoryAccessErr;

    *arg = ram->data + address;
    // sleep
    return RuntimeError::noErr;
}

void processorFree(Processor* proc){
    stack_free(&proc->stack);
    proc->status = ProcStatus::OFF;

    free(proc->code);
}