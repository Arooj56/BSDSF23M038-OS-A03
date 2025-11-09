#include "shell.h"

// execute.c
int execute(char* arglist[], int is_background) {
    int status;
    int pipe_pos = -1;

    // Check for pipe '|'
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0) {
            pipe_pos = i;
            break;
        }
    }

    if (pipe_pos != -1) {
        arglist[pipe_pos] = NULL; // split commands
        char **left_cmd = arglist;
        char **right_cmd = &arglist[pipe_pos + 1];

        int fd[2];
        if (pipe(fd) == -1) { perror("pipe"); return 1; }

        int pid1 = fork();
        if (pid1 == 0) {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]); close(fd[1]);
            execvp(left_cmd[0], left_cmd);
            perror("exec left command");
            exit(1);
        }

        int pid2 = fork();
        if (pid2 == 0) {
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]); close(fd[1]);
            execvp(right_cmd[0], right_cmd);
            perror("exec right command");
            exit(1);
        }

        close(fd[0]); close(fd[1]);
        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);
        return 0;
    }

    // No pipe: check for redirection
    int in = -1, out = -1;
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "<") == 0 && arglist[i+1] != NULL) {
            in = open(arglist[i+1], O_RDONLY);
            if (in < 0) { perror("open input file"); return 1; }
            for (int j = i; arglist[j-1] != NULL; j++) arglist[j] = arglist[j+2];
            i--;
        }
        if (strcmp(arglist[i], ">") == 0 && arglist[i+1] != NULL) {
            out = open(arglist[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (out < 0) { perror("open output file"); return 1; }
            for (int j = i; arglist[j-1] != NULL; j++) arglist[j] = arglist[j+2];
            i--;
        }
    }

    int pid = fork();
    if (pid == 0) {
        if (in != -1) { dup2(in, STDIN_FILENO); close(in); }
        if (out != -1) { dup2(out, STDOUT_FILENO); close(out); }
        execvp(arglist[0], arglist);
        perror("Command not found");
        exit(1);
    } else if (pid > 0) {
        if (is_background) {
			bg_pids[bg_count++] = pid;
			printf("[BG] PID %d started\n", pid);
		} else {
			waitpid(pid, &status, 0);
		}
        return 0;
    } else {
        perror("fork failed");
        return 1;
    }

}