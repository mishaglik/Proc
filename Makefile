LIB_DIR = ./lib
BUILD_DIR = ./bin
SRC_DIR = ./src
CFLAGS = `cat $(LIB_DIR)/Cflags`
SANFLAGS = `cat $(LIB_DIR)/SanitizeFlags`
SOURCES_PROC = Proc.cpp main.cpp
SOURCES_ASM  = Assembler.cpp main.cpp
SOURCES_DASM = 
LIBRARIES = Logger Stack File

all: assembler processor disassembler

common: Programm/commands.cpp Programm/commands.h Programm/Programm.cpp Programm/Programm.h
	g++ Programm/commands.cpp -c -o build/commands.o -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES))
	g++ Programm/Programm.cpp -c -o build/Programm.o -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES))
	ar rvs build/common.a  build/commands.o build/Programm.o 
assembler: common
	g++ Assembler/main.cpp Assembler/Assembler.cpp build/common.a -o assembler -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES)) 
disassembler:

processor: common
	g++ Processor/main.cpp Processor/Proc.cpp build/common.a -o build/processor -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES)) 
clean:
	rm $(BUILD_DIR)/*