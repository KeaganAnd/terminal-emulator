#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>


#include "types.h"
#include "shaders.h"
#include "font.h"
#include "renderer.h"
#include "textHandler.h"
#include "globals.h"
#include "shell.h"
#include "terminal_logic.h"
#include "input.h"


extern Character Characters[128];
extern GLuint VAO, VBO;

unsigned short screenWidth = 800;
unsigned short screenHeight = 600;

int bufferScreenHeight, bufferScreenWidth;
float xScale, yScale;

// Cursor blinking
static double blink_timer = 0.0;
static double last_time = 0.0;
static int cursor_visible = 1;
#define BLINK_INTERVAL 0.5  // 500ms blink interval

// Configuration: whether to render non-ASCII Nerd Font glyphs
// When false, we will skip drawing them but still advance cursor width.
// Later, when multi-font support is added, set this true to attempt rendering.
static bool nerd_font_enabled = true;



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
    last_time = glfwGetTime();
    
    // Set up input callbacks
    // Shell is created below; callbacks will be finalized after shell launch

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
        loadedFont = loadFont("/home/keagan/.local/share/fonts/SpaceMonoNerdFontMono-Regular.ttf");
        //loadedFont = loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
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
    TextBuffer* textBuffer = createTextBuffer();
    if(!textBuffer) {fprintf(stderr, "Text buffer not alocated"); exit(1);}
    fprintf(stderr, "Text Handler Created\n");
    
    TerminalGrid termGrid = createTerminalGrid();
   

    char shellPath[] = "/bin/bash";
    ShellPTY shell = launch_shell(shellPath);
    
    // Finalize input callbacks now that shell is available
    setup_input_callbacks(window, &shell);

    char temp[1024];
    ParserState parser_state = {0};  // Initialize parser state
    parser_state.fg_color = -1;
    parser_state.bg_color = -1;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Update cursor blink using frame delta time
        double now = glfwGetTime();
        double dt = now - last_time;
        last_time = now;
        blink_timer += dt;
        if (blink_timer >= BLINK_INTERVAL) {
            blink_timer -= BLINK_INTERVAL;
            cursor_visible = !cursor_visible;
        }
        
        glClearColor(COLOR4_BLACK.r, COLOR4_BLACK.g, COLOR4_BLACK.b, COLOR4_BLACK.a);
        glClear(GL_COLOR_BUFFER_BIT);
        ssize_t n = shell_receive(&shell, temp, sizeof(temp)-1);

        if (n > 0) {
            temp[n] = '\0';
            // Parse raw bytes in real-time into grid
            process_output_bytes(&termGrid, temp, n, &parser_state);
        }

        // Render the grid every frame so cursor blinks regardless of shell output
        renderGrid(shader, &termGrid, nerd_font_enabled, cursor_visible);
        printBuffer(textBuffer, shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeTextBuffer(textBuffer);
    freeGrid(&termGrid);


    for (int i=0; i<128; i++) glDeleteTextures(1, &Characters[i].TextureID);
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}

