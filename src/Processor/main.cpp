#include "Proc.h"

int main(const int argc, char* argv[]){
    // logger_set_file("log.txt");
    logger_set_abort_level(WARNING);
    logger_set_log_level(ERROR);
    if(argc < 2){
        LOG_MESSAGE_F(ERROR, "No argumetns given. Nothing to execute\n");
    }

    Processor proc = {};
    if(processorLoad(&proc, argv[1])){
        LOG_MESSAGE_F(ERROR, "Error loading processor\n");
        return 0;
    }
    
    RuntimeError err = processorExecute(&proc);
    LOG_MESSAGE_F(INFO, "Programm finished with code: %d\n", err);

    processorFree(&proc);

    return 0;
}