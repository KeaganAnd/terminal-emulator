#include "font.h"
#include "renderer.h"
#include "shaders.h"
#include "types.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include "globals.h"

/** Global array storing all loaded character glyphs */
Character Characters[128];

short fontSize = 14;
/**
 * Loads a TrueType font file and generates OpenGL textures for all ASCII characters (0-127)
 * Creates a glyph texture for each character, sets up texture parameters, and stores metric
 * information (width, height, bearing, advance) in the Characters array.
 * Sets font size to 48 pixels.
 * @param fontPath - Path to the .ttf font file to load
 * @return 1 on success, 0 on failure (FreeType initialization error, file not found, etc)
 */

int getFontSize(){return fontSize;}

int loadFont(const char* fontPath) {
    FT_Library ft;
    FT_Face face;
    if (FT_Init_FreeType(&ft)) { fprintf(stderr,"Could not init FreeType\n"); return 0; }
    if (FT_New_Face(ft, fontPath, 0, &face)) {
        fprintf(stderr,"Failed to load font '%s'\n", fontPath);
        FT_Done_FreeType(ft);
        return 0;
    }

    FT_Set_Pixel_Sizes(face, 0, yScale * fontSize); 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c=0; c<128; c++){
        if (FT_Load_Char(face,c,FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT)) {
            fprintf(stderr, "Missing glyph for char '%c'\n", c);
            continue;
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Characters[c].TextureID = tex;
        Characters[c].Width = face->glyph->bitmap.width;
        Characters[c].Height = face->glyph->bitmap.rows;
        Characters[c].BearingX = face->glyph->bitmap_left;
        Characters[c].BearingY = face->glyph->bitmap_top;
        Characters[c].Advance = face->glyph->advance.x;

        printf("[DEBUG] Loaded char '%c' (ASCII %d) size=%dx%d\n",
               c, c, Characters[c].Width, Characters[c].Height);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return 1;
}
