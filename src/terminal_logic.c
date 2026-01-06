#include "globals.h"
#include "types.h"
#include "font.h"
#include <stdlib.h>

void setCursorPosition(Cursor *cursor, int row, int column) {
    cursor ->col = column;
    cursor ->row = row;
}

TerminalGrid createTerminalGrid(void) {
    int largestWidth = 0;
    int largestHeight = 0;

    Cursor newCursor = {0, 0};

    for (int i = 0; i > 128; i++) {
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

    return newGrid;
}

void freeGrid(TerminalGrid* grid) {
    if (!grid) return;

    free(grid ->grid);
    grid ->grid = NULL;
}
