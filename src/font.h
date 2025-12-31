#ifndef FONT_H
#define FONT_H

#include "types.h"

/**
 * Font loading and glyph management module
 * Uses FreeType library to load font files and generate texture atlases for characters
 */

/** Global array of loaded character glyphs (supports 128 ASCII characters) */
extern Character Characters[128];

/**
 * Loads a TrueType font file and generates OpenGL textures for all ASCII characters
 * Initializes FreeType, loads the font file, generates a 48-pixel texture for each character,
 * and stores glyph metrics in the Characters array
 * @param fontPath - Path to the .ttf font file
 * @return 1 on success, 0 on failure
 */
int loadFont(const char* fontPath);
int getFontSize();
#endif // FONT_H
