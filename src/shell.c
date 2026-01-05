
#include "shell.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#ifdef __APPLE__
    #include <util.h>
#else
    #include <pty.h>
#endif

ShellPTY launch_shell(const char* shell_path) {
    ShellPTY shell = {0};

    // forkpty opens a new PTY and forks
    shell.child_pid = forkpty(&shell.master_fd, NULL, NULL, NULL);
    if (shell.child_pid < 0) {
        perror("forkpty failed");
        exit(1);
    }

    if (shell.child_pid == 0) {
        // Child process: exec shell
        execl(shell_path, shell_path, NULL);
        perror("execl failed");
        exit(1);
    }

    // Parent process: master_fd is ready
    // Optional: make master_fd non-blocking
    int flags = fcntl(shell.master_fd, F_GETFL, 0);
    fcntl(shell.master_fd, F_SETFL, flags | O_NONBLOCK);

    return shell;
}

void shell_send(ShellPTY* shell, const char* input) {
    if (!shell) return;
    write(shell->master_fd, input, strlen(input));
    write(shell->master_fd, "\n", 1); // simulate Enter
}

ssize_t shell_receive(ShellPTY* shell, char* buffer, size_t bufsize) {
    if (!shell) return -1;
    ssize_t n = read(shell->master_fd, buffer, bufsize - 1);
    if (n > 0) buffer[n] = '\0'; // null terminate
    return n;
}

void shell_close(ShellPTY* shell) {
    if (!shell) return;
    close(shell->master_fd);
    kill(shell->child_pid, SIGTERM);
    waitpid(shell->child_pid, NULL, 0);
}

void strip_ansi(char* line) {
    char* src = line;
    char* dst = line;
    while (*src) {
        if (*src == 27 && *(src+1) == '[') {
            // skip until a letter (m, K, etc.)
            src += 2;
            while ((*src < '@' || *src > '~') && *src) src++;
            if (*src) src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}
