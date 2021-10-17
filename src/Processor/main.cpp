#include "Proc.h"

int main(const int argc, char* argv[]){
    // logger_set_file("log.txt");
    logger_set_abort_level(WARNING);
    logger_set_log_level(DEBUG);
    if(argc < 2){
        LOG_MESSAGE_F(ERROR, "No argumetns given. Nothing to execute\n");
    }

    executeFile(argv[1]);
    return 0;
}