#include "textHandler.h"
#include "renderer.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float startYPosition;
float lineSpacing;
unsigned short linePadding;
unsigned int maxLineLength;
unsigned int maxLines;


TextBuffer* createTextBuffer() {
	TextBuffer* tb = malloc(sizeof(TextBuffer));
	tb->count = 0;
    tb ->head = 0;
	tb -> lines = malloc(sizeof(char*) * maxLines);
	for(int i = 0; i < maxLines; i++) {
		tb -> lines[i] = malloc(sizeof(char) * maxLineLength);
		tb -> lines[i][0] = '\0';
	}
    return tb;
}

void freeTextBuffer(TextBuffer* tb) {
    for(int i = 0; i < maxLines; i++) {
        free(tb -> lines[i]);
    }
    free(tb -> lines);
    free(tb);

}

void addToBuffer(TextBuffer* tb, char* text) {
    if ((tb ->count + 1) > maxLines){
        tb -> head = tb ->count;
        tb ->count = 0;
        
    } 
    strncpy(tb ->lines[tb ->count++], text, maxLineLength);
    
}

void printBuffer(TextBuffer* tb, GLuint shader) {
    
    short screenLineSize = (int)((bufferScreenHeight - linePadding) / (fontSize + linePadding));
    int currentYPosition = bufferScreenHeight - lineSpacing;
    

    for (int i = 0; i < screenLineSize; i++) {
        if (tb ->lines[tb ->head + i][0] != '\0') {
            renderText(shader, tb -> lines[tb ->head + i], 25.0f, currentYPosition, 1.0f, COLOR_WHITE);
            currentYPosition -= lineSpacing;
            continue;
        }
        break;
    }
}

void initTextHandler() {
	lineSpacing = (fontSize + 3) * yScale;
	startYPosition = bufferScreenHeight - lineSpacing;
	maxLines = 50;
	maxLineLength = 100;
    linePadding = 3;
}


