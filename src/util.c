#include <time.h>
#include <stdio.h>

#include <ncurses.h>

#include "chip8.h"
#include "window.h"
#include "util.h"

bool g_shouldExit = 0;

struct timespec tmpSleepTime;

bool loadRom(const char *romPath) {
    FILE *rom;
    if (!(rom = fopen(romPath, "rb")))
            return false;

    fseek(rom, 0, SEEK_END);

    if (ftell(rom) > C8_MEM_TOTAL - C8_MEM_RESERVED)
        return false;

    fseek(rom, 0, SEEK_SET);
    fread(g_c8.mem + C8_MEM_RESERVED, 1, C8_MEM_TOTAL - C8_MEM_RESERVED, rom);
    fclose(rom);
    return true;
}

void sleepSec(double sec) {
    tmpSleepTime.tv_nsec = sec * 1e9;
    tmpSleepTime.tv_sec = 0;
    nanosleep(&tmpSleepTime, NULL);
}

void pause() {
    mvprintw(C8_DSP_HEIGHT + 2, 0, "PAUSED");
    while (getch() != ' ')
        sleepSec(1.0 / 60);
    move(C8_DSP_HEIGHT + 2, 0);
    clrtoeol();
}

void initC8() {
    initscr();
    raw();
    noecho();
    timeout(0);
    curs_set(0);
    initWindow();
}

void exitC8() {
    exitWindow();
    endwin();
}

