CC = gcc
CFLAGS = -Wall -g -I include
SRC = src/main.c src/shell.c src/execute.c
OUT = bin/myshell
LDFLAGS = -lreadline

all: $(OUT)

$(OUT): $(SRC)
	@$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	@rm -f $(OUT)
