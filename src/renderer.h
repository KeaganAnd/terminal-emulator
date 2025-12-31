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
 * @param a - Alphas Channel (0.0 to 1.0)
 */

// Breaks down the color values for passing into gl functions
#define COLOR3_BREAKDOWN(color) = (c).r, (c).g, (c).b
#define COLOR4_BREAKDOWN(color) = (c).r, (c).g, (c).b, (c).a

// Color structs for alpha and non alpha colors
typedef struct{
    float r;
    float g;
    float b;
    float a;
}color4;

typedef struct{
    float r;
    float g;
    float b;
} color3;

// Pre made colors
static const color3 COLOR_RED = {1.0f, 0.0f, 0.0f};
static const color3 COLOR_GREEN = {0.0f, 1.0f, 0.0f};
static const color3 COLOR_BLUE = {0.0f, 0.0f, 1.0f};
static const color3 COLOR_WHITE = {1.0f, 1.0f, 1.0f};
static const color3 COLOR_BLACK = {0.0f, 0.0f, 0.0f};

static const color4 COLOR4_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};

void renderText(GLuint shader, const char* text, float x, float y, float scale, color3 color);

#endif // RENDERER_H
