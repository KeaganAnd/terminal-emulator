#include "input.h"
#include <stdio.h>
#include <string.h>

static ShellPTY* s_shell = NULL;
static char input_buffer[256] = {0};
static size_t input_pos = 0;

static void char_callback(GLFWwindow* window, unsigned int codepoint) {
    if (!s_shell) return;
    if (input_pos < sizeof(input_buffer) - 1) {
        input_buffer[input_pos++] = (char)codepoint;
        input_buffer[input_pos] = '\0';
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (!s_shell) return;

    if (key == GLFW_KEY_ENTER) {
        input_buffer[input_pos] = '\0';
        shell_send(s_shell, input_buffer);
        input_pos = 0;
        memset(input_buffer, 0, sizeof(input_buffer));
    } else if (key == GLFW_KEY_BACKSPACE) {
        if (input_pos > 0) {
            input_pos--;
            input_buffer[input_pos] = '\0';
        }
    } else if (key == GLFW_KEY_TAB) {
        if (input_pos < sizeof(input_buffer) - 1) {
            input_buffer[input_pos++] = '\t';
            input_buffer[input_pos] = '\0';
        }
    }
}

void setup_input_callbacks(GLFWwindow* window, ShellPTY* shell) {
    s_shell = shell;
    glfwSetCharCallback(window, char_callback);
    glfwSetKeyCallback(window, key_callback);
}

const char* input_get_buffer() {
    return input_buffer;
}

size_t input_get_length() {
    return input_pos;
}
