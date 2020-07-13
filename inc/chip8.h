#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include <stdint.h>

#include "util.h"

#define C8_MEM_TOTAL 0x1000
#define C8_MEM_RESERVED 0x200
#define C8_STACK_SIZE 16
#define C8_DSP_WIDTH 64
#define C8_DSP_HEIGHT 32
#define C8_CLOCK_SPEED 600
#define C8_FPS 60
#define C8_CLOCKS_PER_FRAME (C8_CLOCK_SPEED / C8_FPS)
#define C8_CHARACTERS { \
    0xF0, 0x90, 0x90, 0x90, 0xF0, \
    0x20, 0x60, 0x20, 0x20, 0x70, \
    0xF0, 0x10, 0xF0, 0x80, 0xF0, \
    0xF0, 0x10, 0xF0, 0x10, 0xF0, \
    0x90, 0x90, 0xF0, 0x10, 0x10, \
    0xF0, 0x80, 0xF0, 0x10, 0xF0, \
    0xF0, 0x80, 0xF0, 0x90, 0xF0, \
    0xF0, 0x10, 0x20, 0x40, 0x40, \
    0xF0, 0x90, 0xF0, 0x90, 0xF0, \
    0xF0, 0x90, 0xF0, 0x10, 0xF0, \
    0xF0, 0x90, 0xF0, 0x90, 0x90, \
    0xE0, 0x90, 0xE0, 0x90, 0xE0, \
    0xF0, 0x80, 0x80, 0x80, 0xF0, \
    0xE0, 0x90, 0x90, 0x90, 0xE0, \
    0xF0, 0x80, 0xF0, 0x80, 0xF0, \
    0xF0, 0x80, 0xF0, 0x80, 0x80, \
}

extern struct C8_c8 {
    u16 stack[C8_STACK_SIZE];
    u16 i:12;
    u16 pc:12;
    u8  mem[C8_MEM_TOTAL];
    u8  dsp[C8_DSP_WIDTH * C8_DSP_HEIGHT / 8];
    u8  keys[16];
    u8  v[16];
    u8  sp;
    u8  dt, st;
} g_c8;

enum eC8Keys {
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

void c8UpdateTimers();
void c8Clock();

#endif

