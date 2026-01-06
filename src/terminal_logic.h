#include "types.h"

void setCursorPosition(Cursor *cursor, int row, int column);
TerminalGrid createTerminalGrid(void);
void freeGrid(TerminalGrid* grid);
