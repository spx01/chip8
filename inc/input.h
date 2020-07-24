#ifndef CHIP8_INPUT_H
#define CHIP8_INPUT_H

#include <stdbool.h>

#define KEY_BUFFER_LIMIT 1
#define KEY_BUFFER_CLEAR_FRAMES 10

void updateKeyBufferFrame();
void handleInput();

#endif
