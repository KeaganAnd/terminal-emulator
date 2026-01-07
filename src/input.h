#pragma once
#include <GLFW/glfw3.h>
#include "shell.h"

void setup_input_callbacks(GLFWwindow* window, ShellPTY* shell);

// Accessors for current input buffer so renderer can overlay typed text
const char* input_get_buffer();
size_t input_get_length();
