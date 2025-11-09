#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    rl_attempted_completion_function = command_completion;

    while ((cmdline = readline(PROMPT)) != NULL) {
        if (strlen(cmdline) == 0) {
            free(cmdline);
            continue;
        }

        // Handle !n re-execution
        if (cmdline[0] == '!') {
            int n = atoi(cmdline + 1);
            if (n > 0 && n <= history_count) {
                free(cmdline);
                cmdline = strdup(history[n - 1]);
                printf("Re-executing: %s\n", cmdline);
            } else {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }
        }

        add_history(cmdline);   // Readline history
        add_to_history(cmdline); // Our optional array history

        if ((arglist = tokenize(cmdline)) != NULL) {
            if (!handle_builtin(arglist))
                execute(arglist);

            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);
            free(arglist);
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}


    
