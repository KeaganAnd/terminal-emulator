#include "textHandler.h"
#include "renderer.h"
#include "font.h"

//idk wtf im doing need to get the font size from the global variable somehow FIX
int fontSize2 = 14;
float currentYPosition = 575.0f;


void printLine(const char* text, GLuint shader) {
    float lineSpacing = fontSize2+2;
   renderText(shader, text, 25.0f, currentYPosition, 1.0f, COLOR_WHITE);
   currentYPosition -= lineSpacing;
}
