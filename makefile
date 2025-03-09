# Compiler and linker
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./includes
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = includes
BIN_DIR = bin
EXEC_DIR = $(BIN_DIR)/exec

# Files
SOURCES = $(SRC_DIR)/rat_source.cpp $(SRC_DIR)/debug_main.cpp
HEADERS = $(INC_DIR)/rat_source.hpp $(INC_DIR)/token.hpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Output
EXEC = $(BIN_DIR)/exec  # Specify the name of the output executable

# Default target to build the executable
all: $(EXEC)

# Rule to build the final executable
$(EXEC): $(OBJECTS)
	@mkdir -p $(BIN_DIR)  # Create the exec directory if it doesn't exist
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Rule to compile source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BIN_DIR) $(EXEC)

# Rule to create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: all clean
