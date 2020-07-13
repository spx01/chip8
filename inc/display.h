#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#define PRINT_FILL {waddch(c8Win, ACS_BLOCK); waddch(c8Win, ACS_BLOCK);}
#define PRINT_EMPTY {waddstr(c8Win, "  ");}
#define PRINT_PIXEL_WIDTH 2

void initDisplay();
void drawDisplay();
void exitDisplay();

#endif

