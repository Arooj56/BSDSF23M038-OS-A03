#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "FCIT> "
#define HISTORY_SIZE 20
#define MAX_BG 20

// Function prototypes
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char* arglist[], int is_background);
int handle_builtin(char **args);
void add_to_history(const char* cmdline);
char* command_completion_generator(const char* text, int state);
char** command_completion(const char* text, int start, int end);
char** split_commands(char* line, int* count);
void handle_sigchld(int sig);

extern char* history[HISTORY_SIZE];
extern int history_count;
extern int bg_pids[MAX_BG];
extern int bg_count;

#endif // SHELL_H
