// shell_pty.h
#pragma once
#include <stddef.h>
#include <sys/types.h>

typedef struct ShellPTY {
    int master_fd;   // PTY master
    pid_t child_pid; // shell process PID
} ShellPTY;

// Launch a shell, return ShellPTY struct
ShellPTY launch_shell(const char* shell_path);

// Send a command string to the shell
void shell_send(ShellPTY* shell, const char* input);

// Read output from the shell into buffer, returns number of bytes read
ssize_t shell_receive(ShellPTY* shell, char* buffer, size_t bufsize);

// Close the shell and free resources
void shell_close(ShellPTY* shell);

void strip_ansi(char* line);

void strip_shell_prompt(char* line);
