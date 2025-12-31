#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "types.h"
#include "shaders.h"
#include "font.h"
#include "renderer.h"

extern Character Characters[128];
extern GLuint VAO, VBO;

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Text Debug", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr,"GLAD init failed\n"); return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    if (!loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf")) {
        fprintf(stderr,"Font load failed\n"); return -1;
    }

    float projection[16] = {0};
    projection[0] = 2.0f / 800.0f;
    projection[5] = 2.0f / 600.0f;
    projection[10] = -1.0f;
    projection[12] = -1.0f;
    projection[13] = -1.0f;
    projection[15] = 1.0f;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"), 1, GL_FALSE, projection);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render yellow text
        renderText(shader, "Hi Sylv!", 25.0f, 550.0f, 1.0f, 1.0f, 1.0f, 0.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i=0; i<128; i++) glDeleteTextures(1, &Characters[i].TextureID);
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

