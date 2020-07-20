#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#define PRINT_FILL {wattron(c8Win, A_REVERSE); waddstr(c8Win, "  ");}
#define PRINT_EMPTY {wattroff(c8Win, A_REVERSE); waddstr(c8Win, "  ");}
#define PRINT_PIXEL_WIDTH 2

void initWindow();
void updateWindow();
void exitWindow();

#endif
