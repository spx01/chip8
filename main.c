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

struct C8_c8 {
    uint8_t  dsp[C8_DSP_WIDTH * C8_DSP_HEIGHT / 8];
    uint16_t stack[C8_STACK_SIZE];
    uint8_t  v[16];
    uint16_t i:12;
    uint16_t pc:12;
    uint16_t keys;
    uint8_t  sp;
    uint8_t  dt, st;
    uint8_t  mem[C8_MEM_TOTAL];
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

    c8.pc = C8_MEM_RESERVED;
    for (;;) {
        static struct timespec sleepTime;
        static int clocksPerFrame = C8_CPU_CLK / C8_REFRESH, clockCnt;
        clock_t clockStart;
        clockStart = clock();

        if (clockCnt == clocksPerFrame) {
            clockCnt = 0;
            c8.dt -= c8.dt > 0;
            c8.st -= c8.st > 0;

            move(0, 0);
            for (uint8_t i = 0; i < C8_DSP_HEIGHT; ++i) {
                for (uint8_t j = 0; j < C8_DSP_WIDTH / 8; ++j)
                    for (uint8_t k = 128; k > 0; k >>= 1) {
                        if (c8.dsp[i * (C8_DSP_WIDTH / 8) + j] & k)
                            addch('\xdb');
                        else
                            addch(' ');
                    }
                addch('\n');
            }
        }

        int ch;
        int otherKeys = 0, totalKeys = 0;
        uint16_t op = c8.mem[c8.pc] << 8 | c8.mem[c8.pc + 1];
        uint8_t x = (op & 0xF00) >> 8, y = (op & 0xF0) >> 4;

        c8.pc += 2;
        c8.keys = 0;
        while ((ch = getch()) != ERR) {
            switch (ch) {
            case '1':
                c8.keys |= 1 << C8_K00;
                break;
            case '2':
                c8.keys |= 1 << C8_K01;
                break;
            case '3':
                c8.keys |= 1 << C8_K02;
                break;
            case '4':
                c8.keys |= 1 << C8_K03;
                break;
            case 'q':
                c8.keys |= 1 << C8_K10;
                break;
            case 'w':
                c8.keys |= 1 << C8_K11;
                break;
            case 'e':
                c8.keys |= 1 << C8_K12;
                break;
            case 'r':
                c8.keys |= 1 << C8_K13;
                break;
            case 'a':
                c8.keys |= 1 << C8_K20;
                break;
            case 's':
                c8.keys |= 1 << C8_K21;
                break;
            case 'd':
                c8.keys |= 1 << C8_K22;
                break;
            case 'f':
                c8.keys |= 1 << C8_K23;
                break;
            case 'z':
                c8.keys |= 1 << C8_K30;
                break;
            case 'x':
                c8.keys |= 1 << C8_K31;
                break;
            case 'c':
                c8.keys |= 1 << C8_K32;
                break;
            case 'v':
                c8.keys |= 1 << C8_K33;
                break;
            case 'c' & 0x1F:
                goto END;
            default:
                ++otherKeys;
            }
            ++totalKeys;
        }

        switch (op >> 12) {
        case 0x0:
            // super chip-48
            // if (op >> 4 == 0xC) {
            //     // 00Cn scd nibble
            //     break;
            // }

            switch (op & 0xFF) {
            case 0xE0:
                // 00E0 cls
                memset(c8.dsp, 0, sizeof(c8.dsp));
                break;
            case 0xEE:
                // 00EE ret
                c8.pc = c8.stack[c8.sp--];
                break;

            // super chip-48
            // case 0xFB:
            //     // 00FB scr
            //     break;
            // case 0xFC:
            //     // 00FC scl
            //     break;
            // case 0xFD:
            //     // 00FD exit
            //     break;
            // case 0xFE:
            //     // 00FE low
            //     break;
            // case 0xFF:
            //     // 00FF high
            //     break;

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
            // super chip-48
            // if (!(op & 0xF)) {
            //     // Dxy0 drw vx vy 0
            //     break;
            // }

            // Dxyn drw vx vy nibble
            uint16_t pos = c8.v[y] * C8_DSP_WIDTH + c8.v[x];
            for (uint8_t i = 0; i < (uint8_t)(op & 0xF); ++i, pos += C8_DSP_WIDTH) {
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
                c8.pc += 2 * (c8.keys >> c8.v[x] & 1);
                break;
            case 0xA1:
                // ExA1 sknp vx
                c8.pc += 2 * !(c8.keys >> c8.v[x] & 1);
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

            // super chip-48
            // case 0x30:
            //     // Fx30 ld hf vx
            //     break;
            // case 0x75:
            //     // Fx75 ld r vx
            //     break;
            // case 0x85:
            //     // Fx85 ld vx r
            //     break;

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
    endwin();
    return 0;
}
