#include <glad/glad.h>

void printLine(const char* text, GLuint shader);
void initTextHandler();

typedef struct {
    char** lines; // 2D array, array of strings(array)
    int count; // The amount of lines currently in the buffer
    int head; // Where the lines should start printing from
} TextBuffer;

TextBuffer* createTextBuffer();
void freeTextBuffer(TextBuffer* tb);
void printBuffer(TextBuffer* tb, GLuint shader);


