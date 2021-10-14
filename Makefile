LIB_DIR = ./lib
BUILD_DIR = ./build
CFLAGS = `cat $(LIB_DIR)/Cflags`
SANFLAGS = `cat $(LIB_DIR)/SanitizeFlags`
SOURCES_PROC = Proc.cpp main.cpp
SOURCES_ASM  = Assembler.cpp main.cpp
SOURCES_DASM = 
LIBRARIES = Logger Stack File
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES = main

all: $(SOURCES) main
	
main: $(OBJECTS) 
	g++ main.cpp $(addprefix $(BUILD_DIR)/, $(OBJECTS)) -L$(LIB_DIR) $(addprefix -l, $(LIBRARIES)) -o build/$@

common: Programm/commands.cpp Programm/commands.h Programm/Programm.cpp Programm/Programm.h
	g++ Programm/commands.cpp -c -o build/commands.o -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES))
	g++ Programm/Programm.cpp -c -o build/Programm.o -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES))
	ar rvs build/common.a  build/commands.o build/Programm.o 
assembler: common
	g++ Assembler/main.cpp Assembler/Assembler.cpp build/common.a -o build/assembler -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES)) 
disassembler:

processor:

clean:
	rm build/*