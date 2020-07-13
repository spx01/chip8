#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;

extern bool g_shouldExit;

bool loadRom(const char *romPath);
void sleepSec(double sec);
void pause();
void initC8();
void exitC8();

#endif

