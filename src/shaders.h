#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <stdio.h>

/**
 * Shader compilation and linking module
 * Handles compilation of vertex and fragment shaders and linking them into programs
 */

/** Vertex shader source code - Handles text quad positioning and texture coordinate mapping */
const char* vertexShaderSrc =
"#version 330 core\n"
"layout(location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"    TexCoords = vertex.zw;\n"
"}\n";

/** Fragment shader source code - Renders textured quads with alpha blending for text */
const char* fragmentShaderSrc =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main() {\n"
"    float alpha = texture(text, vec2(TexCoords.x, 1.0 - TexCoords.y)).r;\n"
"    FragColor = vec4(textColor, alpha);\n"
"}\n";

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
