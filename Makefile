CC      = clang
CFLAGS  = -Wall -Wextra -Wpedantic -std=gnu11
LDFLAGS = -lncurses
OUTDIR  = out

chip8: $(patsubst %.c, out/%.o, $(wildcard *.c))
	$(CC) $^ -o $@ -O2 $(CFLAGS) $(LDFLAGS)

out/%.o: %.c
	if [ ! -d out ]; then mkdir out; fi
	$(CC) -c $^ -o $@ -O2 $(CFLAGS)

chip8-debug: $(patsubst %.c, out/%-debug.o, $(wildcard *.c))
	$(CC) $^ -o $@ -Og $(CFLAGS) $(LDFLAGS)

out/%-debug.o: %.c
	if [ ! -d out ]; then mkdir out; fi
	$(CC) -c $^ -o $@ -Og $(CFLAGS)

debug: chip8-debug

clean:
	$(RM) chip8 chip8-debug out/*
