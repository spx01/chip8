#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include "chip8.h"
#include "display.h"
#include "input.h"
#include "util.h"

int main(int argc, char **argv) {
    if (argc < 2)
        return -1;
    if (!loadRom(argv[1]))
        return -1;

    initC8();
    for (;;) {
        static u8 clockCnt;
        clock_t clockStart;
        clockStart = clock();

        if (++clockCnt == C8_CLOCKS_PER_FRAME) {
            clockCnt = 0;
            c8UpdateTimers();
            drawDisplay();
            updateKeyBufferFrame();
        }

        handleInput();
        if (g_shouldExit)
            break;

        c8Clock();

        sleepSec(1.0 / C8_CLOCK_SPEED - ((double)(clock() - clockStart)) / CLOCKS_PER_SEC);
    }
    exitC8();
    return 0;
}

