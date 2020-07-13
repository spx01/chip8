#include <ncurses.h>

#include "chip8.h"
#include "input.h"
#include "util.h"

u8 framesSinceLastPress[16];

void updateKeyBufferFrame() {
    for (u8 i = 0; i < 16; ++i)
        g_c8.keys[i] *= ++framesSinceLastPress[i] < KEY_BUFFER_CLEAR_FRAMES;
}

bool validKeys;

void handleInput() {
    int ch;
    u8 totalKeys = 0, otherKeys = 0;
    while ((ch = getch()) != ERR) {
        switch (ch) {
        case '1':
            g_c8.keys[C8_K00] += g_c8.keys[C8_K00] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K00] = 0;
            break;
        case '2':
            g_c8.keys[C8_K01] += g_c8.keys[C8_K01] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K01] = 0;
            break;
        case '3':
            g_c8.keys[C8_K02] += g_c8.keys[C8_K02] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K02] = 0;
            break;
        case '4':
            g_c8.keys[C8_K03] += g_c8.keys[C8_K03] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K03] = 0;
            break;
        case 'q':
            g_c8.keys[C8_K10] += g_c8.keys[C8_K10] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K10] = 0;
            break;
        case 'w':
            g_c8.keys[C8_K11] += g_c8.keys[C8_K11] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K11] = 0;
            break;
        case 'e':
            g_c8.keys[C8_K12] += g_c8.keys[C8_K12] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K12] = 0;
            break;
        case 'r':
            g_c8.keys[C8_K13] += g_c8.keys[C8_K13] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K13] = 0;
            break;
        case 'a':
            g_c8.keys[C8_K20] += g_c8.keys[C8_K20] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K20] = 0;
            break;
        case 's':
            g_c8.keys[C8_K21] += g_c8.keys[C8_K21] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K21] = 0;
            break;
        case 'd':
            g_c8.keys[C8_K22] += g_c8.keys[C8_K22] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K22] = 0;
            break;
        case 'f':
            g_c8.keys[C8_K23] += g_c8.keys[C8_K23] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K23] = 0;
            break;
        case 'z':
            g_c8.keys[C8_K30] += g_c8.keys[C8_K30] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K30] = 0;
            break;
        case 'x':
            g_c8.keys[C8_K31] += g_c8.keys[C8_K31] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K31] = 0;
            break;
        case 'c':
            g_c8.keys[C8_K32] += g_c8.keys[C8_K32] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K32] = 0;
            break;
        case 'v':
            g_c8.keys[C8_K33] += g_c8.keys[C8_K33] < KEY_BUFFER_LIMIT;
            framesSinceLastPress[C8_K33] = 0;
            break;
        case 'c' & 0x1F:
            g_shouldExit = 1;
            break;
        case ' ':
            pause();
        default:
            ++otherKeys;
        }
        ++totalKeys;
    }
    validKeys = totalKeys != otherKeys;
}

