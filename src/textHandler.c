#include "textHandler.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void printBuffer(TextBuffer* tb, GLuint shader) {
    for (int i = 0; i < tb->count; i++) {
//        printf("%s\n", tb->lines[i]);
        printf("%lu\n", strlen(tb->lines[i]));
        for (int x = 0; x < strlen(tb->lines[i]); x++) {
            if(tb->lines[i][x] == '\b') {printf("Found new line\n");}
            printf("%c", tb->lines[i][x]);
        }
        printf("\n");
    } 


}

void initTextHandler() {
	maxLines = 50;
	maxLineLength = 100;
}


