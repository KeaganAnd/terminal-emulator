#include "types.h"
#include <stddef.h>
#include <sys/types.h>

typedef struct {
	int cursor_row;
	int cursor_col;
	int fg_color;
	int bg_color;
	int bold;
	int underline;
} ParserState;

void setCursorPosition(Cursor *cursor, int row, int column);
TerminalGrid createTerminalGrid(void);
void freeGrid(TerminalGrid* grid);
void writeCell(TerminalGrid* grid, int x, int y, uint32_t rune, color3* fg, color3* bg);
void setCursorPosition(Cursor *cursor, int row, int column);

// Parse a CSI escape starting at raw (if present), return bytes consumed
int parse_escape_sequence(const char* raw, size_t len, ParserState* state);

// Map ANSI color codes to RGB
color3 get_color_from_code(int color_code);

// Process a chunk of shell output into grid with ANSI handling
void process_output_bytes(TerminalGrid* grid, const char* buf, ssize_t n, ParserState* state);

// Clear the terminal grid (for ESC[2J sequences)
void clear_screen(TerminalGrid* grid);
