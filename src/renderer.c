#include "renderer.h"
#include "types.h"
#include <stdio.h>

/** Vertex Array Object - stores vertex buffer configuration for text quads */
GLuint VAO;
/** Vertex Buffer Object - stores quad vertex data for rendering characters */
GLuint VBO;

/** Reference to the global character array loaded by the font module */
extern Character Characters[128];

/**
 * Renders a text string to the screen at the specified position and color
 * For each character: calculates position and size based on glyph metrics and scale,
 * creates quad vertices, uploads them to GPU, binds the glyph texture, and draws the quad.
 * Cursor position is advanced for each character based on the advance metric.
 * @param shader - Compiled shader program to use for rendering
 * @param text - Null-terminated string to render
 * @param x - X position in screen coordinates
 * @param y - Y position in screen coordinates
 * @param scale - Scale factor for rendering (1.0 = normal size)
 * @param r - Red color component (0.0 to 1.0)
 * @param g - Green color component (0.0 to 1.0)
 * @param b - Blue color component (0.0 to 1.0)
 */


void renderText(GLuint shader, const char* text, float x, float y, float scale, color3 color) {
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader,"textColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (const char* c=text; *c; c++) {
        Character ch = Characters[(unsigned char)(*c)];
        if(ch.TextureID == 0) continue;

        float xpos = x + ch.BearingX * scale;
        float ypos = y - (ch.Height - ch.BearingY) * scale;
        float w = ch.Width * scale;
        float h = ch.Height * scale;

        float vertices[6][4] = {  
    {xpos,     ypos,     0.0f, 0.0f},
    {xpos,     ypos + h, 0.0f, 1.0f},
    {xpos + w, ypos + h, 1.0f, 1.0f},

    {xpos,     ypos,     0.0f, 0.0f},
    {xpos + w, ypos + h, 1.0f, 1.0f},
    {xpos + w, ypos,     1.0f, 0.0f}
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
