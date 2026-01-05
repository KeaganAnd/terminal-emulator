#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <string.h>

#ifdef __APPLE__
    #include <util.h>
#else
    #include <pty.h>
#endif


#include "types.h"
#include "shaders.h"
#include "font.h"
#include "renderer.h"
#include "textHandler.h"
#include "globals.h"
#include "shell.h"


extern Character Characters[128];
extern GLuint VAO, VBO;

unsigned short screenWidth = 800;
unsigned short screenHeight = 600;

int bufferScreenHeight, bufferScreenWidth;
float xScale, yScale;

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Mag Terminal", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr,"GLAD init failed\n"); return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwGetFramebufferSize(window, &bufferScreenWidth, &bufferScreenHeight);
    glfwGetWindowContentScale(window, &xScale, &yScale);
    

    GLuint shader = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    int loadedFont;

    #if defined(_WIN32)
    // Windows (both 32 and 64 bit)
        loadedFont = loadFont("C:/Windows/Fonts/consola.ttf");
    #elif defined(__APPLE__) && defined(__MACH__)
	loadedFont = loadFont("/System/Library/Fonts/Menlo.ttc");
    #elif defined(__linux__) || defined(__unix__) || defined(__posix__)
        loadedFont = loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
    #else
        // Unknown OS
        fptrintf(stderr,"Text is not setup for this OS yet\n");
    #endif

    if (!loadedFont) {
        fprintf(stderr,"Font load failed\n"); return -1;
    }

    float projection[16] = {0};
    projection[0] = 2.0f / bufferScreenWidth;
    projection[5] = 2.0f / bufferScreenHeight;
    projection[10] = -1.0f;
    projection[12] = -1.0f;
    projection[13] = -1.0f;
    projection[15] = 1.0f;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"), 1, GL_FALSE, projection);

    
    initTextHandler(); 
    fprintf(stderr, "Text Handler Init\n");
    TextBuffer* textBuffer = createTextBuffer();
    fprintf(stderr, "Text Handler Created\n");
    
    char shellPath[] = "/bin/zsh";
    //TESTING SHELL
    ShellPTY shell = launch_shell(shellPath);


    char commandSent[] = "echo Hello World";
    shell_send(&shell, commandSent);
    shell_send(&shell, commandSent);
    shell_send(&shell, commandSent);

    char temp[1024];
    #define ACCUM_SIZE 1024
    static char line_accum[ACCUM_SIZE];
    static size_t line_len = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(COLOR4_BLACK.r, COLOR4_BLACK.g, COLOR4_BLACK.b, COLOR4_BLACK.a); //Background color color4 object
        ssize_t n = shell_receive(&shell, temp, sizeof(temp)-1);

        if (n > 0) {
        temp[n] = '\0'; // null-terminate for safety

        // accumulate characters
        for (ssize_t i = 0; i < n; i++) {
            char c = temp[i];

            if (c == '\n') {
                line_accum[line_len] = '\0';
                if (strncmp(line_accum, commandSent, strlen(commandSent)) == 0) {
                   // ignore echoed input
                } else {
                    strip_ansi(line_accum);
                    addToBuffer(textBuffer, line_accum);
                }
                line_len = 0; // reset accumulator
            } else if (c != '\r') { // optional: ignore carriage returns
                if (line_len < ACCUM_SIZE - 1) {
                    line_accum[line_len++] = c;
                    }
                }
            }
        }

        printBuffer(textBuffer, shader); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeTextBuffer(textBuffer);

    for (int i=0; i<128; i++) glDeleteTextures(1, &Characters[i].TextureID);
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}

