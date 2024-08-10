CC=gcc
CFLAGS = -Wall -pedantic -std=c90

# Directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Executable name
EXEC = assembler

# Default target
all: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

# Phony targets
.PHONY: all clean
