# Compiler and Flags
CC = gcc
CFLAGS = -Iinclude -Wall

# Folders
SRC = src
OBJ = obj
BIN = bin

# Files
TARGET = $(BIN)/myshell
SRC_FILES = $(SRC)/main.c $(SRC)/shell.c $(SRC)/execute.c
OBJ_FILES = $(OBJ)/main.o $(OBJ)/shell.o $(OBJ)/execute.o

# Default rule
all: $(TARGET)

# Build target
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET)

# Compile source files to object files
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ)/*.o $(TARGET)

