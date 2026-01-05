#ifndef GLOBALS_H
#define GLOBALS_H

// @param screenWidth - Width Of The Screen | ushort
// @param screenHeight - Height Of The Screen | ushort
// @param fontSize - Size of text on screen | short 
// @param maxLines - Maximum amount of lines held in the buffer | uint 
// @param maxLineLength - The maximum amount of characters held in a single line | uint 
// @param linePadding - The amount of pixels between lines of text | ushort 
extern short fontSize;
extern int bufferScreenWidth;
extern int bufferScreenHeight;
extern float xScale;
extern float yScale;

extern unsigned int maxLines;
extern unsigned int maxLineLength;
extern unsigned short linePadding;

#endif
