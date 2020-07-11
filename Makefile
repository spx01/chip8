CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=gnu11
LDFLAGS = -lncurses

chip8: main.c
	$(CC) $^ -o $@ -O2 $(CFLAGS) $(LDFLAGS)
chip8-debug: main.c
	$(CC) $^ -o $@ -Og $(CFLAGS) $(LDFLAGS)
debug: chip8-debug
clean:
	$(RM) chip8 chip8-debug
