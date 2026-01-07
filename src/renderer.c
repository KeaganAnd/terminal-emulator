#include "renderer.h"
#include "globals.h"
#include "types.h"
#include "font.h"
#include "input.h"
#include <math.h>

/** Vertex Array Object - stores vertex buffer configuration for text quads */
GLuint VAO;
/** Vertex Buffer Object - stores quad vertex data for rendering characters */
GLuint VBO;

/** Reference to the global character array loaded by the font module */
extern Character Characters[128];
extern float xScale, yScale;

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
 */


void renderText(GLuint shader, const char* text, float x, float y, float scale, color3 color) {
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader,"textColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    

    for (const char* c=text; *c; c++) {
        Character ch = Characters[(unsigned char)(*c)];
        if(ch.TextureID == 0) continue;

        float xpos = floorf(x + ch.BearingX * scale);
        float ypos = floorf(y - (ch.Height - ch.BearingY) * scale);
        float w = floorf(ch.Width * scale);
        float h = floorf(ch.Height * scale);

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

void renderGlyph(GLuint shader, const Character* chPtr, float x, float y, float scale, color3 color) {
    if (!chPtr || chPtr->TextureID == 0) return;
    Character ch = *chPtr;
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader,"textColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float xpos = floorf(x + ch.BearingX * scale);
    float ypos = floorf(y - (ch.Height - ch.BearingY) * scale);
    float w = floorf(ch.Width * scale);
    float h = floorf(ch.Height * scale);

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

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderGrid(GLuint shader, TerminalGrid* grid, bool nerd_font_enabled, bool cursor_visible) {
    extern short fontSize;
    float line_spacing = (fontSize + 3) * yScale;
    int cell_advance = getCellAdvance();

    for (int row = 0; row < grid->height; row++) {
        float y = bufferScreenHeight - (row + 1) * line_spacing;
        float x = 0;

        for (int col = 0; col < grid->width; col++) {
            int idx = row * grid->width + col;
            Cell* cell = &grid->grid[idx];

            // cursor drawn after full pass

            if (cell->rune == 0) {
                x += cell_advance;
                continue;
            }

            if (cell->rune < 128) {
                char char_str[2] = {(char)cell->rune, '\0'};
                renderText(shader, char_str, x, y, 1.0f, cell->fg);
            } else if (nerd_font_enabled) {
                const Character* g = getGlyph(cell->rune);
                if (g) {
                    renderGlyph(shader, g, x, y, 1.0f, cell->fg);
                }
            }

            x += cell_advance;
        }
    }

    // Overlay user input buffer next to the last prompt
    const char* inbuf = input_get_buffer();
    size_t inlen = input_get_length();
    if (inbuf && inlen > 0) {
        int row = grid->cursor.row;
        int col = grid->cursor.col;
        if (row < 0) row = 0; if (row >= grid->height) row = grid->height - 1;
        if (col < 0) col = 0; if (col > grid->width) col = grid->width;
        float y = bufferScreenHeight - (row + 1) * line_spacing;
        float x = (float)(col * cell_advance);

        for (size_t i = 0; i < inlen; i++) {
            unsigned char ch = (unsigned char)inbuf[i];
            if (ch < 128) {
                char s[2] = {(char)ch, '\0'};
                renderText(shader, s, x, y, 1.0f, COLOR_WHITE);
            } else if (nerd_font_enabled) {
                const Character* g = getGlyph((uint32_t)ch);
                if (g) renderGlyph(shader, g, x, y, 1.0f, COLOR_WHITE);
            }
            x += cell_advance;
        }
    }

    // Draw blinking cursor at insertion position
    if (cursor_visible) {
        int row = grid->cursor.row;
        int col = grid->cursor.col + (int)inlen; // insertion point after typed text
        
        if (row >= 0 && row < grid->height && col >= 0 && col < grid->width) {
            float y = bufferScreenHeight - (row + 1) * line_spacing;
            float x = (float)(col * cell_advance);
            
            // Draw a solid block cursor using the full block character
            const Character* block = getGlyph(0x2588); // U+2588 FULL BLOCK
            if (block) {
                renderGlyph(shader, block, x, y, 1.0f, COLOR_WHITE);
            }
        }
    }
}
