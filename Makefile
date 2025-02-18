# Compiler
CPP := g++
CPPFLAGS := -Wall -Wextra -std=c++17 -Iinclude

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN := $(BUILD_DIR)/app

# Find all .cpp files in src/ and subdirectories
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(BIN)

# Link all object files into the final executable
$(BIN): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CPP) $(CPPFLAGS) $^ -o $@

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

# Clean the build directory
clean:
	rm -rf $(BUILD_DIR)

# Run the executable
run: all
	./$(BIN)

# PHONY targets
.PHONY: all clean run
