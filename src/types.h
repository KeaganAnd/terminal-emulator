#ifndef TYPES_H
#define TYPES_H

#include <glad/glad.h>

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

#endif // TYPES_H
