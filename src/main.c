#include "shell.h"

int bg_pids[MAX_BG];
int bg_count = 0;


void handle_sigchld(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("\n[BG] PID %d finished\n", pid);
        fflush(stdout);

        // Re-display prompt after message
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

int main() {
	signal(SIGCHLD, handle_sigchld);
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

        add_history(cmdline);    // readline history
        add_to_history(cmdline); // our own array history

        int cmd_count = 0;
        char** commands = split_commands(cmdline, &cmd_count);

        for (int i = 0; i < cmd_count; i++) {
            char* cmd = commands[i];
            int is_bg = 0;

            int len = strlen(cmd);
            if (len > 0 && cmd[len - 1] == '&') {
                is_bg = 1;
                cmd[len - 1] = '\0';
            }

            if ((arglist = tokenize(cmd)) != NULL) {
                if (!handle_builtin(arglist)) {
                    execute(arglist, is_bg);
                }

                for (int j = 0; arglist[j] != NULL; j++)
                    free(arglist[j]);
                free(arglist);
            }

            free(cmd);
        }
        free(commands);

        // Reap finished background jobs
        int status;
        for (int i = 0; i < bg_count; i++) {
            pid_t result = waitpid(bg_pids[i], &status, WNOHANG);
            if (result > 0) {
                printf("[BG] PID %d finished\n", bg_pids[i]);
                for (int j = i; j < bg_count - 1; j++)
                    bg_pids[j] = bg_pids[j + 1];
                bg_count--;
                i--;
            }
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
