LIB_DIR = ./lib
BUILD_DIR = ./build
CFLAGS = `cat $(LIB_DIR)/Cflags`
SANFLAGS = `cat $(LIB_DIR)/SanitizeFlags`
SOURCES_PROC = Proc.cpp main.cpp
SOURCES_ASM  = Assembler.cpp main.cpp
SOURCES_DASM = 
LIBRARIES = Logger Stack
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES = main

all: $(SOURCES) main
	
main: $(OBJECTS) 
	g++ main.cpp $(addprefix $(BUILD_DIR)/, $(OBJECTS)) -L$(LIB_DIR) $(addprefix -l, $(LIBRARIES)) -o build/$@

assembler:
	g++ Assembler/main.cpp Assembler/Assembler.cpp -o build/assembler commands.cpp -L$(LIB_DIR) $(CFLAGS) $(addprefix -l, $(LIBRARIES))
disassembler:
processor:

.cpp.o:
	g++ -c $(CFLAGS) $(SANFLAGS) $< -o $(BUILD_DIR)/$@

clean:
	rm build/*