#include "globals.h"
#include "renderer.h"
#include "types.h"
#include "font.h"
#include "terminal_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setCursorPosition(Cursor *cursor, int row, int column) {
    cursor ->col = column;
    cursor ->row = row;
}

void writeCell(TerminalGrid* grid, int x, int y, uint32_t rune, color3* fg, color3* bg) {
    grid ->grid[y * grid ->width + x].rune = rune;
    if(fg) grid ->grid[y * grid ->width + x].fg = *fg;
    if (bg) grid ->grid[y * grid ->width + x].bg = *bg;
}

void writeAtCursor(TerminalGrid* grid, char rune, color3* fg, color3* bg) {
    if (!grid) return;

    int x = grid ->cursor.col;
    int y = grid ->cursor.row;

    if (x < 0 || x >= grid ->width) return;
    if (y < 0 || y >= grid ->height) return;

    Cell* c = &grid ->grid[y * grid->width + x];

    c ->rune = rune;
    if (fg) c->fg = *fg;
    if (bg) c->bg = *bg;
}

TerminalGrid createTerminalGrid(void) {
    int largestWidth = 1;
    int largestHeight = 1;

    Cursor newCursor = {0, 0};

    for (int i = 0; i < 128; i++) {
        if (Characters[i].Width > largestWidth) {largestWidth = Characters[i].Width;}
        if (Characters[i].Height > largestHeight) {largestHeight = Characters[i].Height;}
    } // If array size for characters changes this needs to change too

    int numOfHoriCells = bufferScreenWidth/largestWidth;
    int numOfVertCells = bufferScreenHeight/largestHeight;

    TerminalGrid newGrid;


    newGrid.cursor = newCursor;
    newGrid.height = numOfVertCells;
    newGrid.width = numOfHoriCells;
    newGrid.grid = malloc(sizeof(Cell) * numOfHoriCells * numOfVertCells);

    if (!newGrid.grid) {
        fprintf(stderr, "Grid array could not be allocated");
        abort();
    }

    for (int y = 0; y < numOfVertCells; y++) {
        for (int x = 0; x < numOfHoriCells; x++) {
            setCursorPosition(&newGrid.cursor, x, y);            
            writeAtCursor(&newGrid, ' ', &COLOR_WHITE, &COLOR_BLACK);
        }
    }

    return newGrid;
}

void freeGrid(TerminalGrid* grid) {
    if (!grid) return;

    free(grid ->grid);
    grid ->grid = NULL;
}

void clear_screen(TerminalGrid* grid) {
    if (!grid || !grid->grid) return;
    for (int i = 0; i < grid->width * grid->height; i++) {
        grid->grid[i].rune = 0;
        grid->grid[i].fg = (color3){1.0f, 1.0f, 1.0f};
        grid->grid[i].bg = (color3){0.0f, 0.0f, 0.0f};
        grid->grid[i].flags = 0;
    }
}

static color3 ansi_colors[8] = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
};

color3 get_color_from_code(int color_code) {
    if (color_code >= 0 && color_code < 8) return ansi_colors[color_code];
    return ansi_colors[7];
}

// Decode UTF-8 sequence starting at buf[pos], return codepoint and bytes consumed
static int decode_utf8(const char* buf, size_t len, size_t pos, uint32_t* codepoint) {
    if (pos >= len) return 0;
    unsigned char c = (unsigned char)buf[pos];
    
    // Single byte (ASCII)
    if ((c & 0x80) == 0) {
        *codepoint = c;
        return 1;
    }
    // 2-byte sequence
    if ((c & 0xE0) == 0xC0) {
        if (pos + 1 >= len) return 0;
        unsigned char c1 = (unsigned char)buf[pos + 1];
        *codepoint = ((c & 0x1F) << 6) | (c1 & 0x3F);
        return 2;
    }
    // 3-byte sequence
    if ((c & 0xF0) == 0xE0) {
        if (pos + 2 >= len) return 0;
        unsigned char c1 = (unsigned char)buf[pos + 1];
        unsigned char c2 = (unsigned char)buf[pos + 2];
        *codepoint = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        return 3;
    }
    // 4-byte sequence
    if ((c & 0xF8) == 0xF0) {
        if (pos + 3 >= len) return 0;
        unsigned char c1 = (unsigned char)buf[pos + 1];
        unsigned char c2 = (unsigned char)buf[pos + 2];
        unsigned char c3 = (unsigned char)buf[pos + 3];
        *codepoint = ((c & 0x07) << 18) | ((c1 & 0x3F) << 12) | 
                     ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        return 4;
    }
    // Invalid sequence, treat as single byte
    *codepoint = c;
    return 1;
}

int parse_escape_sequence(const char* raw, size_t len, ParserState* state) {
    if (len < 2 || raw[0] != 27 || raw[1] != '[') return 0;
    const char* p = raw + 2;
    int param1 = 0, param2 = 0;
    char final_byte = '\0';

    while ((size_t)(p - raw) < len && *p && !(*p >= '@' && *p <= '~')) {
        if (*p >= '0' && *p <= '9') {
            param1 = param1 * 10 + (*p - '0');
        } else if (*p == ';') {
            param2 = param1;
            param1 = 0;
        }
        p++;
    }
    if ((size_t)(p - raw) < len) {
        final_byte = *p;
        p++;
    }

    switch (final_byte) {
        case 'H':
        case 'f':
            // CSI row;col H  (row is first parameter)
            state->cursor_row = (param1 > 0) ? param1 - 1 : 0;
            state->cursor_col = (param2 > 0) ? param2 - 1 : 0;
            break;
        case 'A':
            state->cursor_row -= (param1 > 0) ? param1 : 1;
            if (state->cursor_row < 0) state->cursor_row = 0;
            break;
        case 'B':
            state->cursor_row += (param1 > 0) ? param1 : 1;
            break;
        case 'C':
            state->cursor_col += (param1 > 0) ? param1 : 1;
            break;
        case 'D':
            state->cursor_col -= (param1 > 0) ? param1 : 1;
            if (state->cursor_col < 0) state->cursor_col = 0;
            break;
        case 'J':
            // Clear display: 0=cursor to end, 1=start to cursor, 2=entire screen
            // For now, treat all as clear entire screen (most common with 'clear' command)
            // Store this in state so process_output_bytes can handle it
            state->fg_color = -2; // Use -2 as a signal to clear screen
            break;
        case 'm':
            if (param1 == 0) {
                state->fg_color = -1;
                state->bg_color = -1;
                state->bold = 0;
                state->underline = 0;
            } else if (param1 >= 30 && param1 <= 37) {
                state->fg_color = param1 - 30;
            } else if (param1 >= 40 && param1 <= 47) {
                state->bg_color = param1 - 40;
            } else if (param1 == 1) {
                state->bold = 1;
            } else if (param1 == 4) {
                state->underline = 1;
            }
            break;
    }
    return (int)(p - raw);
}

void process_output_bytes(TerminalGrid* grid, const char* temp, ssize_t n, ParserState* state) {
    for (ssize_t i = 0; i < n; ) {
        unsigned char c = (unsigned char)temp[i];
        // Check if clear screen was signaled
        if (state->fg_color == -2) {
            clear_screen(grid);
            state->fg_color = -1; // Reset signal
        }
        if (c == '\r') { i++; continue; }
        if (c == '\n') {
            state->cursor_row++;
            state->cursor_col = 0;
            grid->cursor.row = state->cursor_row;
            grid->cursor.col = state->cursor_col;
            i++;
            continue;
        }
        if (c == 27) {
            int consumed = parse_escape_sequence(temp + i, (size_t)(n - i), state);
            if (consumed > 0) { i += consumed; continue; }
        }
        
        // Decode UTF-8 character
        uint32_t codepoint = 0;
        int bytes_consumed = decode_utf8(temp, (size_t)n, (size_t)i, &codepoint);
        if (bytes_consumed > 0) {
            if (state->cursor_row >= 0 && state->cursor_col >= 0) {
                color3 fg = get_color_from_code(state->fg_color >= 0 ? state->fg_color : 7);
                color3 bg = get_color_from_code(state->bg_color >= 0 ? state->bg_color : 0);
                writeCell(grid, state->cursor_col, state->cursor_row, codepoint, &fg, &bg);
                state->cursor_col++;
                grid->cursor.row = state->cursor_row;
                grid->cursor.col = state->cursor_col;
            }
            i += bytes_consumed;
        } else {
            i++; // Skip invalid byte
        }
    }
}

