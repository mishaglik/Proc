LIB_DIR = ./lib
BUILD_DIR = ./build
CFLAGS = `cat $(LIB_DIR)/Cflags`
SOURCES = Proc.cpp
LIBRARIES = Logger Stack
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES = main

all: $(SOURCES) main
	
main: $(OBJECTS) 
	$(cat OBJECTS)
	g++ main.cpp $(addprefix $(BUILD_DIR)/, $(OBJECTS)) -L$(LIB_DIR) $(addprefix -l, $(LIBRARIES)) -o build/$@

.cpp.o:
	g++ -c $(CFLAGS) $< -o $(BUILD_DIR)/$@

clean:
	rm build/*