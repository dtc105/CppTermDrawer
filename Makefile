# Compiler
CPP := g++
CPPFLAGS := -Wall -Wextra -std=c++17 -Iinclude

# Directories
SRC_DIR := src
BUILD_DIR := build
FRAME_DIR := frames
TXT_FILES := $(wildcard $(FRAME_DIR)/text/*.txt)
PNG_FILES := $(patsubst $(FRAME_DIR)/text/%.txt, $(FRAME_DIR)/img/%.png, $(TXT_FILES))
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
	rm $(BUILD_DIR)/*

$(FRAME_DIR)/img/%.png: $(FRAME_DIR)/text/%.txt
	mkdir -p $(FRAME_DIR)/img
	magick -density 300 -size 1440x1440! -background transparent -fill white \
	-font "Courier-New" -pointsize 8 -gravity center label:@$< $@

gif: $(PNG_FILES)
	magick -size 1440x1440 -delay 10 -dispose background -loop 0 $(shell ls -v $(FRAME_DIR)/img/*.png) $(FRAME_DIR)/animation.gif

# Clear frames and gifs
clear:
	rm $(FRAME_DIR)/text/*
	rm $(FRAME_DIR)/img/*
	rm $(FRAME_DIR)/animation.gif

# Run the executable
run: all
	./$(BIN)

# PHONY targets
.PHONY: all clean run
