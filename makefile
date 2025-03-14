# compiler and linker
CXX = g++
CXXFLAGS = -Wall -std=c++17 -g -O0 -I./includes 
LDFLAGS = 

# colors
BLUE = \033[1;34m
GREEN = \033[1;32m
RESET = \033[0m
PURPLE = \033[1;35m

# directories
SRC_DIR = src
INC_DIR = includes
BIN_DIR = bin
EXEC_DIR = $(BIN_DIR)/exec
TEST_DIR = tests

# files
SOURCES = $(SRC_DIR)/rat_source.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/test.cpp
HEADERS = $(INC_DIR)/rat_source.hpp $(INC_DIR)/token.hpp $(INC_DIR)/parser.hpp $(INC_DIR)/test.hpp $(INC_DIR)/ast.hpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# output
EXEC = $(BIN_DIR)/exec
# TEST_EXEC = $(BIN_DIR)/exec

# default target to build the executable
all: $(EXEC)

# build executable
$(EXEC): $(OBJECTS)
	@mkdir -p $(BIN_DIR)  # create the bin directory if it doesn't exist
	@echo "$(PURPLE)$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)$(RESET)"
	@ $(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "$(GREEN)Linking Complete...$(RESET)"

# build test executable
$(TEST_EXEC): $(TEST_OBJECTS) $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "$(PURPLE)$(CXX) $(TEST_OBJECTS) $(OBJECTS) -o $@ $(LDFLAGS)$(RESET)"
	@ $(CXX) $(TEST_OBJECTS) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "$(GREEN)Linking test executable...$(RESET)"

# compile source into object
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BIN_DIR)
	@echo "$(GREEN)Compiling $<$(RESET)"
	@echo "$(PURPLE)$(CXX) $(CXXFLAGS) -c $< -o $@$(RESET)"
	@ $(CXX) $(CXXFLAGS) -c $< -o $@

# compile test source into object
$(BIN_DIR)/%.o: $(TEST_DIR)/%.cpp $(TEST_HEADERS) | $(BIN_DIR)
	@echo "$(GREEN)Compiling test $<$(RESET)"
	@echo "$(PURPLE)$(CXX) $(CXXFLAGS) -c $< -o $@$(RESET)"
	@ $(CXX) $(CXXFLAGS) -c $< -o $@

# clean up bin 
clean:
	@echo "$(GREEN)Cleaning bin and exec directory...$(RESET)"
	@ rm -rf $(BIN_DIR) $(EXEC) $(TEST_EXEC)
	@echo "$(PURPLE)rm -rf $(BIN_DIR) $(EXEC) $(TEST_EXEC)$(RESET)"

# create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(EXEC)
	@echo "$(BLUE)Running exec...\n$(RESET)"
	@ ./$(EXEC)

# test target
# test: $(TEST_EXEC)
# 	@echo "$(BLUE)Running tests...\n$(RESET)"
# 	@ ./$(TEST_EXEC)

.PHONY: all clean test
