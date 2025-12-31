#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>

/**
 * Text rendering module
 * Handles rendering of text strings using pre-loaded font glyphs and OpenGL
 */

/** Vertex Array Object - stores vertex buffer configuration for text quads */
extern GLuint VAO;
/** Vertex Buffer Object - stores quad vertex data for rendering characters */
extern GLuint VBO;

/**
 * Renders a text string to the screen at the specified position and color
 * Iterates through each character in the string, calculates quad positions based on glyph
 * metrics, uploads vertex data to GPU, and draws textured quads
 * @param shader - Compiled shader program to use for rendering
 * @param text - Null-terminated string to render
 * @param x - X position in screen coordinates
 * @param y - Y position in screen coordinates
 * @param scale - Scale factor for rendering (1.0 = normal size)
 * @param r - Red color component (0.0 to 1.0)
 * @param g - Green color component (0.0 to 1.0)
 * @param b - Blue color component (0.0 to 1.0)
 */
void renderText(GLuint shader, const char* text, float x, float y, float scale, float r, float g, float b);

#endif // RENDERER_H
