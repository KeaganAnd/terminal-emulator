#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <stdio.h>

/**
 * Shader compilation and linking module
 * Handles compilation of vertex and fragment shaders and linking them into programs
 */

/** Vertex shader source code - Handles text quad positioning and texture coordinate mapping */
extern const char* vertexShaderSrc;

/** Fragment shader source code - Renders textured quads with alpha blending for text */
extern const char* fragmentShaderSrc;

/**
 * Compiles a single shader from source code
 * @param type - Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
 * @param src - Shader source code as string
 * @return Compiled shader ID, or 0 on failure
 */
GLuint compileShader(GLenum type, const char* src);

/**
 * Creates a complete shader program by compiling and linking vertex and fragment shaders
 * @param vSrc - Vertex shader source code
 * @param fSrc - Fragment shader source code
 * @return Linked shader program ID, or 0 on failure
 */
GLuint createShaderProgram(const char* vSrc, const char* fSrc);

#endif // SHADERS_H
