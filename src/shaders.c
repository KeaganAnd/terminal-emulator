#include "shaders.h"

/**
 * Compiles a single shader from source code
 * Prints error messages to stderr if compilation fails
 * @param type - Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
 * @param src - Shader source code as string
 * @return Compiled shader ID, or 0 on failure
 */
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        fprintf(stderr, "Shader compile error: %s\n", info);
    }
    return shader;
}

/**
 * Creates a complete shader program by compiling and linking vertex and fragment shaders
 * Prints error messages to stderr if linking fails
 * @param vSrc - Vertex shader source code
 * @param fSrc - Fragment shader source code
 * @return Linked shader program ID, or 0 on failure
 */
GLuint createShaderProgram(const char* vSrc, const char* fSrc) {
    GLuint vert = compileShader(GL_VERTEX_SHADER, vSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(program, 512, NULL, info);
        fprintf(stderr, "Program link error: %s\n", info);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}
