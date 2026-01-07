#include "font.h"
#include "types.h"
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include "globals.h"

/** Global array storing all loaded character glyphs */
Character Characters[128];

// Keep FreeType library and face alive for dynamic glyph loading
static FT_Library g_ft = NULL;
static FT_Face g_face = NULL;

// Simple cache for non-ASCII glyphs
typedef struct {
    uint32_t codepoint;
    Character ch;
} ExtraGlyph;

#define EXTRA_GLYPH_CAP 512
static ExtraGlyph g_extraGlyphs[EXTRA_GLYPH_CAP];
static size_t g_extraCount = 0;

short fontSize = 13;
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
    if (FT_Init_FreeType(&g_ft)) { fprintf(stderr,"Could not init FreeType\n"); return 0; }
    if (FT_New_Face(g_ft, fontPath, 0, &g_face)) {
        fprintf(stderr,"Failed to load font '%s'\n", fontPath);
        FT_Done_FreeType(g_ft);
        g_ft = NULL;
        return 0;
    }

    // Select Unicode character map (critical for Nerd Fonts and special characters)
    if (FT_Select_Charmap(g_face, FT_ENCODING_UNICODE)) {
        fprintf(stderr, "Warning: Could not select Unicode charmap\n");
    }

    FT_Set_Pixel_Sizes(g_face, 0, yScale * fontSize); 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c=0; c<128; c++){
        if (FT_Load_Char(g_face,c,FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT)) {
            fprintf(stderr, "Missing glyph for char '%c'\n", c);
            continue;
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     g_face->glyph->bitmap.width,
                     g_face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE,
                     g_face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Characters[c].TextureID = tex;
        Characters[c].Width = g_face->glyph->bitmap.width;
        Characters[c].Height = g_face->glyph->bitmap.rows;
        Characters[c].BearingX = g_face->glyph->bitmap_left;
        Characters[c].BearingY = g_face->glyph->bitmap_top;
        Characters[c].Advance = g_face->glyph->advance.x;

        printf("[DEBUG] Loaded char '%c' (ASCII %d) size=%dx%d\n",
               c, c, Characters[c].Width, Characters[c].Height);
    }

    // Keep face and library for dynamic glyph loading
    return 1;
}

// Use the space character width as the fixed cell advance (in pixels)
int getCellAdvance() {
    return (Characters[' '].Advance >> 6);
}

// Load a glyph for a Unicode codepoint > 127 and cache it; return pointer or NULL on failure
static const Character* load_extra_glyph(uint32_t codepoint) {
    if (!g_face) return NULL;
    if (g_extraCount >= EXTRA_GLYPH_CAP) return NULL;

    // Get the glyph index for this Unicode codepoint
    FT_UInt glyph_index = FT_Get_Char_Index(g_face, codepoint);
    if (glyph_index == 0) {
        return NULL;  // Glyph not in font, will fallback to replacement char
    }

    // Load the glyph using the glyph index
    if (FT_Load_Glyph(g_face, glyph_index, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT)) {
        return NULL;  // Failed to load, will fallback to replacement char
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                 g_face->glyph->bitmap.width,
                 g_face->glyph->bitmap.rows,
                 0, GL_RED, GL_UNSIGNED_BYTE,
                 g_face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    ExtraGlyph* eg = &g_extraGlyphs[g_extraCount++];
    eg->codepoint = codepoint;
    eg->ch.TextureID = tex;
    eg->ch.Width = g_face->glyph->bitmap.width;
    eg->ch.Height = g_face->glyph->bitmap.rows;
    eg->ch.BearingX = g_face->glyph->bitmap_left;
    eg->ch.BearingY = g_face->glyph->bitmap_top;
    eg->ch.Advance = g_face->glyph->advance.x;

    return &eg->ch;
}

// Public: get glyph for codepoint (ASCII uses Characters[], others loaded on demand)
const Character* getGlyph(uint32_t codepoint) {
    if (codepoint < 128) {
        return &Characters[codepoint];
    }
    // Look in cache
    for (size_t i = 0; i < g_extraCount; i++) {
        if (g_extraGlyphs[i].codepoint == codepoint) return &g_extraGlyphs[i].ch;
    }
    // Load on demand
    const Character* result = load_extra_glyph(codepoint);
    if (result) return result;
    
    // Fallback to '?' if glyph not found
    return &Characters['?'];
}
