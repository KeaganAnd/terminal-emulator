#ifndef TYPES_H
#define TYPES_H

#include "renderer.h"
#include <glad/glad.h>
#include <stdint.h>

/**
 * Character struct - Represents a single character glyph in the font atlas
 * Stores texture data and metrics needed for rendering individual characters
 */
typedef struct {
    GLuint TextureID;      /**< Texture ID for the character glyph */
    int Width;             /**< Width of the character bitmap */
    int Height;            /**< Height of the character bitmap */
    int BearingX;          /**< Offset from cursor x to left edge of glyph */
    int BearingY;          /**< Offset from baseline to top of glyph */
    unsigned int Advance;  /**< Distance to advance cursor for next character */
} Character;

typedef struct {
    uint32_t rune;
    color3 fg;
    color4 bg;
    uint8_t flags;
} Cell;

typedef struct {
    int row;
    int col;
} Cursor;

typedef struct {
    int width;
    int height;
    Cursor cursor;
    Cell *grid;
} TerminalGrid;

#endif // TYPES_H
