#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#define C8_MEM_TOTAL 0x1000
#define C8_MEM_RESERVED 0x200
#define C8_STACK_SIZE 16
#define C8_DSP_WIDTH 64
#define C8_DSP_HEIGHT 32
#define C8_CPU_CLK 600
#define C8_REFRESH 60

#define KEY_BUFFER_LIMIT 5
#define KEY_BUFFER_CLEAR_FRAMES 10
#define PRINT_FILL {waddch(win, ACS_BLOCK); waddch(win, ACS_BLOCK);}
#define PRINT_EMPTY {waddstr(win, "  ");}
#define PIXEL_CHAR_WIDTH 2

struct C8_c8 {
    uint16_t stack[C8_STACK_SIZE];
    uint16_t i:12;
    uint16_t pc:12;
    uint8_t  mem[C8_MEM_TOTAL];
    uint8_t  dsp[C8_DSP_WIDTH * C8_DSP_HEIGHT / 8];
    uint8_t  keys[16];
    uint8_t  v[16];
    uint8_t  sp;
    uint8_t  dt, st;
} c8 = {.mem = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
}};

enum eC8_keys {
    C8_K31,
    C8_K00,
    C8_K01,
    C8_K02,
    C8_K10,
    C8_K11,
    C8_K12,
    C8_K20,
    C8_K21,
    C8_K22,
    C8_K30,
    C8_K32,
    C8_K03,
    C8_K13,
    C8_K23,
    C8_K33
};

bool loadRomFile(const char *filename, uint8_t *ptr, long maxSize) {
    FILE *rom = fopen(filename, "rb");
    fseek(rom, 0, SEEK_END);

    if (ftell(rom) > maxSize)
        return false;

    fseek(rom, 0, SEEK_SET);
    fread(ptr, 1, maxSize, rom);
    fclose(rom);
    return true;
}

int main(int argc, char **argv) {
    if (argc < 2)
        return -1;
    if (!loadRomFile(argv[1], c8.mem + C8_MEM_RESERVED, C8_MEM_TOTAL - C8_MEM_RESERVED))
        return -1;

    initscr();
    raw();
    noecho();
    timeout(0);
    curs_set(0);

    WINDOW *win = newwin(C8_DSP_HEIGHT + 2, C8_DSP_WIDTH * PIXEL_CHAR_WIDTH + 2, 0, 0);
    box(win, 0, 0);

    c8.pc = C8_MEM_RESERVED;
    for (;;) {
        static struct timespec sleepTime;
        static uint8_t clocksPerFrame = C8_CPU_CLK / C8_REFRESH, clockCnt;
        static uint8_t framesSinceLastPress[16];
        clock_t clockStart;
        clockStart = clock();

        if (clockCnt == clocksPerFrame) {
            clockCnt = 0;
            c8.dt -= c8.dt > 0;
            c8.st -= c8.st > 0;

            wmove(win, 1, 1);
            for (uint8_t i = 0; i < C8_DSP_HEIGHT; ++i) {
                for (uint8_t j = 0; j < C8_DSP_WIDTH / 8; ++j)
                    for (uint8_t k = 128; k > 0; k >>= 1) {
                        if (c8.dsp[i * (C8_DSP_WIDTH / 8) + j] & k)
                            PRINT_FILL
                        else
                            PRINT_EMPTY
                    }
                wmove(win, i + 2, 1);
            }
            wrefresh(win);

            for (uint8_t i = 0; i < 16; ++i)
                if (framesSinceLastPress[i]++ == KEY_BUFFER_CLEAR_FRAMES)
                    c8.keys[i] = 0;
        }

        int ch;
        int otherKeys = 0, totalKeys = 0;
        uint16_t op = c8.mem[c8.pc] << 8 | c8.mem[c8.pc + 1];
        uint8_t x = (op & 0xF00) >> 8, y = (op & 0xF0) >> 4;

        c8.pc += 2;
        while ((ch = getch()) != ERR) {
            switch (ch) {
            case '1':
                c8.keys[C8_K00] += c8.keys[C8_K00] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K00] = 0;
                break;
            case '2':
                c8.keys[C8_K01] += c8.keys[C8_K01] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K01] = 0;
                break;
            case '3':
                c8.keys[C8_K02] += c8.keys[C8_K02] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K02] = 0;
                break;
            case '4':
                c8.keys[C8_K03] += c8.keys[C8_K03] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K03] = 0;
                break;
            case 'q':
                c8.keys[C8_K10] += c8.keys[C8_K10] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K10] = 0;
                break;
            case 'w':
                c8.keys[C8_K11] += c8.keys[C8_K11] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K11] = 0;
                break;
            case 'e':
                c8.keys[C8_K12] += c8.keys[C8_K12] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K12] = 0;
                break;
            case 'r':
                c8.keys[C8_K13] += c8.keys[C8_K13] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K13] = 0;
                break;
            case 'a':
                c8.keys[C8_K20] += c8.keys[C8_K20] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K20] = 0;
                break;
            case 's':
                c8.keys[C8_K21] += c8.keys[C8_K21] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K21] = 0;
                break;
            case 'd':
                c8.keys[C8_K22] += c8.keys[C8_K22] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K22] = 0;
                break;
            case 'f':
                c8.keys[C8_K23] += c8.keys[C8_K23] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K23] = 0;
                break;
            case 'z':
                c8.keys[C8_K30] += c8.keys[C8_K30] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K30] = 0;
                break;
            case 'x':
                c8.keys[C8_K31] += c8.keys[C8_K31] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K31] = 0;
                break;
            case 'c':
                c8.keys[C8_K32] += c8.keys[C8_K32] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K32] = 0;
                break;
            case 'v':
                c8.keys[C8_K33] += c8.keys[C8_K33] < KEY_BUFFER_LIMIT;
                framesSinceLastPress[C8_K33] = 0;
                break;
            case 'c' & 0x1F:
                goto END;
            case KEY_RESIZE:
                break;
            default:
                ++otherKeys;
            }
            ++totalKeys;
        }

        switch (op >> 12) {
        case 0x0:
            switch (op & 0xFF) {
            case 0xE0:
                // 00E0 cls
                memset(c8.dsp, 0, sizeof(c8.dsp));
                break;
            case 0xEE:
                // 00EE ret
                c8.pc = c8.stack[c8.sp--];
                break;
            default:
                goto UNK_OP;
            }
            break;
        case 0x1:
            // 1nnn jp addr
            c8.pc = op & 0xFFF;
            break;
        case 0x2:
            // 2nnn call addr
            c8.stack[++c8.sp] = c8.pc;
            c8.pc = op & 0xFFF;
            break;
        case 0x3:
            // 3xkk se vx byte
            c8.pc += 2 * (c8.v[x] == (op & 0xFF));
            break;
        case 0x4:
            // 4xkk sne vx byte
            c8.pc += 2 * (c8.v[x] != (op & 0xFF));
            break;
        case 0x5:
            // 5xy0
            c8.pc += 2 * (c8.v[x] == c8.v[y]);
            break;
        case 0x6:
            // 6xkk ld vx byte
            c8.v[x] = op & 0xFF;
            break;
        case 0x7:
            // 7xkk add vx byte
            c8.v[x] += op & 0xFF;
            break;
        case 0x8:
            switch (op & 0xF) {
            case 0x0:
                // 8xy0 ld vx vy
                c8.v[x] = c8.v[y];
                break;
            case 0x1:
                // 8xy1 or vx vy
                c8.v[x] |= c8.v[y];
                break;
            case 0x2:
                // 8xy2 and vx vy
                c8.v[x] &= c8.v[y];
                break;
            case 0x3:
                // 8xy3 xor vx vy
                c8.v[x] ^= c8.v[y];
                break;
            case 0x4: {
                // 8xy4 add vx vy
                uint16_t tmp = c8.v[x] + c8.v[y];
                c8.v[0xF] = tmp > 0xFF;
                c8.v[x] = tmp & 0xFF;
                break;
            }
            case 0x5:
                // 8xy5 sub vx vy
                c8.v[0xF] = c8.v[x] > c8.v[y];
                c8.v[x] -= c8.v[y];
                break;
            case 0x6:
                // 8xy6 shr vx {vy}
                c8.v[0xF] = c8.v[x] & 1;
                c8.v[x] >>= 1;
                break;
            case 0x7:
                // 8xy7 subn vx vy
                c8.v[0xF] = c8.v[y] > c8.v[x];
                c8.v[x] = c8.v[y] - c8.v[x];
                break;
            case 0xE:
                // 8xyE shl vx {vy}
                c8.v[0xF] = c8.v[x] >> 7 & 1;
                c8.v[x] <<= 1;
                break;
            default:
                goto UNK_OP;
            }
            break;
        case 0x9:
            // 9xy0 sne vx vy
            c8.pc += 2 * (c8.v[x] != c8.v[y]);
            break;
        case 0xA:
            // Annn ld i addr
            c8.i = op & 0xFFF;
            break;
        case 0xB:
            // Bnnn jp v0 addr
            c8.pc = (op & 0xFFF) + c8.v[0];
            break;
        case 0xC:
            // Cxkk rnd vx byte
            c8.v[x] = op & 0xFF & rand();
            break;
        case 0xD: {
            // Dxyn drw vx vy nibble
            c8.v[0xF] = 0;
            uint16_t pos = c8.v[y] * C8_DSP_WIDTH + c8.v[x];
            for (uint8_t i = 0; i < (uint8_t)(op & 0xF); ++i, pos += C8_DSP_WIDTH, pos %= C8_DSP_WIDTH * C8_DSP_HEIGHT) {
                uint16_t dspBytes = ((uint16_t)c8.dsp[pos / 8] << 8) + c8.dsp[pos / 8 + 1];
                uint16_t memBytes = c8.mem[c8.i + i] << (8 - pos % 8);
                c8.v[0xF] |= (memBytes & dspBytes) != 0;
                dspBytes ^= memBytes;
                c8.dsp[pos / 8] = dspBytes >> 8;
                c8.dsp[pos / 8 + 1] = dspBytes & 0xFF;
            }
            break;
        }
        case 0xE:
            switch (op & 0xFF) {
            case 0x9E:
                // Ex9E skp vx
                c8.pc += 2 * (c8.keys[c8.v[x]]);
                c8.keys[c8.v[x]] -= c8.keys[c8.v[x]] > 0;
                break;
            case 0xA1:
                // ExA1 sknp vx
                c8.pc += 2 * !(c8.keys[c8.v[x]]);
                c8.keys[c8.v[x]] -= c8.keys[c8.v[x]] > 0;
                break;
            default:
                goto UNK_OP;
            }
            break;
        case 0xF:
            switch (op & 0xFF) {
            case 0x07:
                // Fx07 ld vx dt
                c8.v[x] = c8.dt;
                break;
            case 0x0A:
                // Fx0A ld vx k
                c8.pc -= 2 * (totalKeys == otherKeys);
                break;
            case 0x15:
                // Fx15 ld dt vx
                c8.dt = c8.v[x];
                break;
            case 0x18:
                // Fx18 ld st vx
                c8.st = c8.v[x];
                break;
            case 0x1E:
                // Fx1E add i vx
                c8.i += c8.v[x];
                break;
            case 0x29:
                // Fx29 ld f vx
                c8.i = c8.v[x] * 5;
                break;
            case 0x33:
                // Fx33 ld b vx
                c8.mem[c8.i] = c8.v[x] / 100;
                c8.mem[c8.i + 1] = c8.v[x] / 10 % 10;
                c8.mem[c8.i + 2] = c8.v[x] % 10;
                break;
            case 0x55:
                // Fx55 ld [i] vx
                for (uint8_t i = 0; i <= x; ++i)
                    c8.mem[c8.i + i] = c8.v[i];
                break;
            case 0x65:
                // Fx65 ld vx [i]
                for (uint8_t i = 0; i <= x; ++i)
                     c8.v[i] = c8.mem[c8.i + i];
                break;
            default:
                goto UNK_OP;
            }
            break;
        default:
UNK_OP:
           fprintf(stderr, "unknown opcode: %04X\n", op);
        }

        ++clockCnt;
        sleepTime.tv_nsec = 1e9 / C8_CPU_CLK - 1e9 * ((double)(clock() - clockStart) / CLOCKS_PER_SEC);
        nanosleep(&sleepTime, NULL);
    }

END:
    delwin(win);
    endwin();
    return 0;
}
