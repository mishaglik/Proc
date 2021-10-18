LIB_DIR = lib/
BIN_DIR = bin/
SRC_DIR = src/

PRC_DIR = Processor/
ASM_DIR = Assembler/
DSM_DIR = Disassembler/

LIBRARIES = Logger Stack File

CXXFLAGS = `cat $(LIB_DIR)Cflags`
SANFLAGS = `cat $(LIB_DIR)SanitizeFlags`
LXXFLAGS = -L$(LIB_DIR) $(addprefix -l, $(LIBRARIES))

MAJOR_VERSION = 1
MINOR_VERSION = 2
BUILD_VERSION = `cat bld_version`

CXXFLAGS += -DMAJOR_VERSION=$(MAJOR_VERSION)
CXXFLAGS += -DMINOR_VERSION=$(MINOR_VERSION)
CXXFLAGS += -DBUILD_VERSION=$(BUILD_VERSION)

SOURCES_PRC = Proc.cpp
SOURCES_ASM = Assembler.cpp
SOURCES_DSM = 
SOURCES_COM = commands.cpp Programm.cpp

EXECUTABLE  = main.cpp

SRC_PRC = $(addprefix $(PRC_DIR), $(SOURCES_PRC) $(EXECUTABLE))
SRC_ASM = $(addprefix $(ASM_DIR), $(SOURCES_ASM) $(EXECUTABLE))
SRC_DSM = $(addprefix $(DSM_DIR), $(SOURCES_DSM) $(EXECUTABLE))
SRC_COM = 						  $(SOURCES_COM)

OBJ_PRC = $(SRC_PRC:.cpp=.o) 
OBJ_ASM = $(SRC_ASM:.cpp=.o) 
OBJ_DSM = $(SRC_DSM:.cpp=.o) 
OBJ_COM = $(SRC_COM:.cpp=.o)

TARGETS = prc asm dsm

all: $(TARGETS)

prc: $(addprefix $(BIN_DIR), $(OBJ_PRC) $(OBJ_COM))
	g++ $(CXXFLAGS) $^ $(LXXFLAGS) -o $@

asm: $(addprefix $(BIN_DIR), $(OBJ_ASM) $(OBJ_COM))
	g++ $(CXXFLAGS) $^ $(LXXFLAGS) -o $@

dsm: $(addprefix $(BIN_DIR), $(OBJ_DSM) $(OBJ_COM))
	g++ $(CXXFLAGS) $^ $(LXXFLAGS) -o $@

$(BIN_DIR)%.o : $(SRC_DIR)%.cpp
	g++ -c $(CXXFLAGS) $(LXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(BIN_DIR)$(ASM_DIR)* $(BIN_DIR)$(PRC_DIR)* $(BIN_DIR)$(DSM_DIR)*
	rm $(TARGETS)