#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {
		// Handle !n before adding to history
		if (cmdline[0] == '!') {
			int n = atoi(cmdline + 1);
			if (n > 0 && n <= history_count) {
				free(cmdline);
				cmdline = strdup(history[n - 1]);
				printf("Re-executing: %s\n", cmdline);
			} else {
				printf("No such command in history.\n");
				continue;
			}
		}

		add_to_history(cmdline);

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