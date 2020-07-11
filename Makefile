CC = clang
CFLAGS = -O2 -Wall -Wextra -Wpedantic -std=gnu11
LDFLAGS = -lncurses

chip8: main.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
clean:
	$(RM) chip8
