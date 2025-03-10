# Compiler and linker
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./includes
LDFLAGS = 

# Colors
BLUE = \033[1;34m
GREEN = \033[1;32m
RESET = \033[0m
PURPLE = \033[1;35m

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
	@echo "$(PURPLE)$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)$(RESET)"
	@ $(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "$(GREEN)Linking...$(RESET)"

# Rule to compile source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BIN_DIR)
	@echo "$(GREEN)Compiling $<$(RESET)"
	@echo "$(PURPLE)$(CXX) $(CXXFLAGS) -c $< -o $@$(RESET)"
	@ $(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	@echo "$(GREEN)Cleaning bin and exec directory...$(RESET)"
	@ rm -rf $(BIN_DIR) $(EXEC)
	@echo "$(PURPLE)rm -rf $(BIN_DIR) $(EXEC)$(RESET)"

# Rule to create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(EXEC)
	@echo "$(BLUE)Running exec...$(RESET)"
	@ ./$(EXEC)
	
.PHONY: all clean
