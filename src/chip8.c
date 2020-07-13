#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "util.h"

struct C8 g_c8 = {.pc = C8_MEM_RESERVED, .mem = C8_CHARACTERS};

void c8UpdateTimers() {
    g_c8.dt -= g_c8.dt > 0;
    g_c8.st -= g_c8.st > 0;
}

void c8Clock() {
    u16 op = g_c8.mem[g_c8.pc] << 8 | g_c8.mem[g_c8.pc + 1];
    u8 x = (op & 0xF00) >> 8, y = (op & 0xF0) >> 4;
    g_c8.pc += 2;
    switch (op >> 12) {
    case 0x0:
        switch (op & 0xFF) {
        case 0xE0:
            // 00E0 cls
            memset(g_c8.dsp, 0, sizeof(g_c8.dsp));
            break;
        case 0xEE:
            // 00EE ret
            g_c8.pc = g_c8.stack[g_c8.sp--];
            break;
        }
        break;
    case 0x1:
        // 1nnn jp addr
        g_c8.pc = op & 0xFFF;
        break;
    case 0x2:
        // 2nnn call addr
        g_c8.stack[++g_c8.sp] = g_c8.pc;
        g_c8.pc = op & 0xFFF;
        break;
    case 0x3:
        // 3xkk se vx byte
        g_c8.pc += 2 * (g_c8.v[x] == (op & 0xFF));
        break;
    case 0x4:
        // 4xkk sne vx byte
        g_c8.pc += 2 * (g_c8.v[x] != (op & 0xFF));
        break;
    case 0x5:
        // 5xy0
        g_c8.pc += 2 * (g_c8.v[x] == g_c8.v[y]);
        break;
    case 0x6:
        // 6xkk ld vx byte
        g_c8.v[x] = op & 0xFF;
        break;
    case 0x7:
        // 7xkk add vx byte
        g_c8.v[x] += op & 0xFF;
        break;
    case 0x8:
        switch (op & 0xF) {
        case 0x0:
            // 8xy0 ld vx vy
            g_c8.v[x] = g_c8.v[y];
            break;
        case 0x1:
            // 8xy1 or vx vy
            g_c8.v[x] |= g_c8.v[y];
            break;
        case 0x2:
            // 8xy2 and vx vy
            g_c8.v[x] &= g_c8.v[y];
            break;
        case 0x3:
            // 8xy3 xor vx vy
            g_c8.v[x] ^= g_c8.v[y];
            break;
        case 0x4: {
            // 8xy4 add vx vy
            u16 tmp = g_c8.v[x] + g_c8.v[y];
            g_c8.v[0xF] = tmp > 0xFF;
            g_c8.v[x] = tmp & 0xFF;
            break;
        }
        case 0x5:
            // 8xy5 sub vx vy
            g_c8.v[0xF] = g_c8.v[x] > g_c8.v[y];
            g_c8.v[x] -= g_c8.v[y];
            break;
        case 0x6:
            // 8xy6 shr vx {vy}
            g_c8.v[0xF] = g_c8.v[x] & 1;
            g_c8.v[x] >>= 1;
            break;
        case 0x7:
            // 8xy7 subn vx vy
            g_c8.v[0xF] = g_c8.v[y] > g_c8.v[x];
            g_c8.v[x] = g_c8.v[y] - g_c8.v[x];
            break;
        case 0xE:
            // 8xyE shl vx {vy}
            g_c8.v[0xF] = g_c8.v[x] >> 7 & 1;
            g_c8.v[x] <<= 1;
            break;
        }
        break;
    case 0x9:
        // 9xy0 sne vx vy
        g_c8.pc += 2 * (g_c8.v[x] != g_c8.v[y]);
        break;
    case 0xA:
        // Annn ld i addr
        g_c8.i = op & 0xFFF;
        break;
    case 0xB:
        // Bnnn jp v0 addr
        g_c8.pc = (op & 0xFFF) + g_c8.v[0];
        break;
    case 0xC:
        // Cxkk rnd vx byte
        g_c8.v[x] = op & 0xFF & rand();
        break;
    case 0xD: {
        // Dxyn drw vx vy nibble
        g_c8.v[0xF] = 0;
        u16 pos = g_c8.v[y] * C8_DSP_WIDTH + g_c8.v[x];
        for (u8 i = 0; i < (u8)(op & 0xF); ++i, pos += C8_DSP_WIDTH, pos %= C8_DSP_WIDTH * C8_DSP_HEIGHT) {
            u8 tmp = pos / 8;
            u16 dspBytes = ((u16)g_c8.dsp[tmp] << 8) + g_c8.dsp[tmp + 1];
            u16 memBytes = g_c8.mem[g_c8.i + i] << (8 - pos % 8);
            g_c8.v[0xF] |= (memBytes & dspBytes) != 0;
            dspBytes ^= memBytes;
            g_c8.dsp[tmp] = dspBytes >> 8;
            g_c8.dsp[tmp + 1] = dspBytes & 0xFF;
        }
        break;
    }
    case 0xE:
        switch (op & 0xFF) {
        case 0x9E:
            // Ex9E skp vx
            g_c8.pc += 2 * (g_c8.keys[g_c8.v[x]]);
            g_c8.keys[g_c8.v[x]] -= g_c8.keys[g_c8.v[x]] > 0;
            break;
        case 0xA1:
            // ExA1 sknp vx
            g_c8.pc += 2 * !(g_c8.keys[g_c8.v[x]]);
            g_c8.keys[g_c8.v[x]] -= g_c8.keys[g_c8.v[x]] > 0;
            break;
        }
        break;
    case 0xF:
        switch (op & 0xFF) {
        case 0x07:
            // Fx07 ld vx dt
            g_c8.v[x] = g_c8.dt;
            break;
        case 0x0A: {
            // Fx0A ld vx k
            extern bool validKeys;
            g_c8.pc -= 2 * !validKeys;

            extern u8 framesSinceLastPress[16];
            u8 i;
            for (i = 0; framesSinceLastPress[i]; ++i);
            g_c8.v[x] = i;
            break;
        }
        case 0x15:
            // Fx15 ld dt vx
            g_c8.dt = g_c8.v[x];
            break;
        case 0x18:
            // Fx18 ld st vx
            g_c8.st = g_c8.v[x];
            break;
        case 0x1E:
            // Fx1E add i vx
            g_c8.i += g_c8.v[x];
            break;
        case 0x29:
            // Fx29 ld f vx
            g_c8.i = g_c8.v[x] * 5;
            break;
        case 0x33:
            // Fx33 ld b vx
            g_c8.mem[g_c8.i] = g_c8.v[x] / 100;
            g_c8.mem[g_c8.i + 1] = g_c8.v[x] / 10 % 10;
            g_c8.mem[g_c8.i + 2] = g_c8.v[x] % 10;
            break;
        case 0x55:
            // Fx55 ld [i] vx
            for (u8 i = 0; i <= x; ++i)
                g_c8.mem[g_c8.i + i] = g_c8.v[i];
            break;
        case 0x65:
            // Fx65 ld vx [i]
            for (u8 i = 0; i <= x; ++i)
                 g_c8.v[i] = g_c8.mem[g_c8.i + i];
            break;
        }
        break;
    }
}

