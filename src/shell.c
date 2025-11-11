#include "shell.h"

char* history[HISTORY_SIZE];
int history_count = 0;

const char* commands[] = {
    "cd", "exit", "help", "ls", "cat", "pwd", "mkdir", "rmdir", "echo", NULL
};

// Add command to history array (optional; readline also keeps its own)
void add_to_history(const char* cmdline) {
    if (cmdline == NULL || strlen(cmdline) == 0) return;

    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmdline);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++)
            history[i - 1] = history[i];
        history[HISTORY_SIZE - 1] = strdup(cmdline);
    }
}

// Built-in command handler
int handle_builtin(char **args) {
    if (args[0] == NULL) return 0;

    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    }else if (strcmp(args[0], "help") == 0) {
    	printf("Built-in commands:\n");
    	printf("cd, exit, history, jobs, set, help\n");  // <-- added 'set'
    	return 1;
    } else if (strcmp(args[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    } else if (strcmp(args[0], "history") == 0) {
        for (int i = 0; i < history_count; i++) {
            printf("%d %s\n", i + 1, history[i]);
        }
        return 1;
    }

    return 0; // Not a built-in
}

// Tokenize command line
char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0') return NULL;

    char** arglist = malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len, argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++;
        if (*cp == '\0') break;
        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t')) len++;
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    if (argnum == 0) {
        for(int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
    return arglist;
}

// ----------------------
// Completion generator
// ----------------------
char* command_completion_generator(const char* text, int state) {
    static int list_index, len;
    static DIR* dir = NULL;
    static struct dirent* entry;
    const char* name;

    if (!state) { // New completion attempt
        list_index = 0;
        len = strlen(text);
        if (dir) { closedir(dir); dir = NULL; }
    }

    // 1️⃣ Complete built-in commands (only at start)
    if (rl_point == 0 || rl_line_buffer[0] != '\0') {
        while ((name = commands[list_index++]) != NULL) {
            if (strncmp(name, text, len) == 0)
                return strdup(name);
        }
    }

    // 2️⃣ Complete files/directories for any argument
    if (!dir) dir = opendir(".");
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, text, len) == 0)
            return strdup(entry->d_name);
    }

    if (dir) { closedir(dir); dir = NULL; }
    return NULL;
}

// ----------------------
// Tab completion entry point
// ----------------------
char** command_completion(const char* text, int start, int end) {
    (void)end; // unused parameter

    // If cursor is after first word, complete filenames
    if (start > 0)
        return rl_completion_matches(text, rl_filename_completion_function);

    // Otherwise, complete commands + filenames
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, command_completion_generator);
}

char** split_commands(char* line, int* count) {
    char** commands = malloc(sizeof(char*) * (MAXARGS + 1));
    char* token;
    int idx = 0;

    token = strtok(line, ";");
    while (token != NULL && idx < MAXARGS) {
        while (*token == ' ') token++; // skip leading spaces
        commands[idx++] = strdup(token);
        token = strtok(NULL, ";");
    }
    commands[idx] = NULL;
    *count = idx;
    return commands;
}


