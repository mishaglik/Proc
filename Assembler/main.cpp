#include "Assembler.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("No input file specified. Use help for help");
        LOG_ERROR("No input file specified");
    }
    for(int argN = 1; argN < argc; argN++){
        assemblyFile(argv[argN]);
    }
}