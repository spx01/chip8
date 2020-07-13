#include <ncurses.h>

#include "chip8.h"
#include "display.h"
#include "util.h"

WINDOW *c8Win;

void initDisplay() {
    c8Win = newwin(C8_DSP_HEIGHT + 2, C8_DSP_WIDTH * PRINT_PIXEL_WIDTH + 2, 0, 0);
    box(c8Win, 0, 0);
}

void drawDisplay() {
    wmove(c8Win, 1, 1);
    for (u8 i = 0; i < C8_DSP_HEIGHT; ++i) {
        for (u8 j = 0; j < C8_DSP_WIDTH / 8; ++j)
            for (u8 k = 128; k > 0; k >>= 1)
                if (g_c8.dsp[i * (C8_DSP_WIDTH / 8) + j] & k)
                    PRINT_FILL
                else
                    PRINT_EMPTY
        wmove(c8Win, i + 2, 1);
    }
    wrefresh(c8Win);
}

void exitDisplay() {
    delwin(c8Win);
}

